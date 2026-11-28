import Foundation

/// Базовый класс калькулятора, реализующий протокол `MathOperations`.
open class BaseCalculator: MathOperations {
    open var name: String { "Базовый калькулятор" }

    public init() {}

    open func add(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch commonType(lhs, rhs) {
        case .int:
            let (res, overflow) = intValue(lhs).addingReportingOverflow(intValue(rhs))
            guard !overflow else { throw MathError.integerOverflow }
            return .int(res)
        case .float:
            let res = lhs.asFloat + rhs.asFloat
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .float(res)
        case .double:
            let res = lhs.asDouble + rhs.asDouble
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .double(res)
        case .decimal:
            let res = lhs.asDecimal + rhs.asDecimal
            guard !res.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(res)
        }
    }

    open func subtract(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch commonType(lhs, rhs) {
        case .int:
            let (res, overflow) = intValue(lhs).subtractingReportingOverflow(intValue(rhs))
            guard !overflow else { throw MathError.integerOverflow }
            return .int(res)
        case .float:
            let res = lhs.asFloat - rhs.asFloat
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .float(res)
        case .double:
            let res = lhs.asDouble - rhs.asDouble
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .double(res)
        case .decimal:
            let res = lhs.asDecimal - rhs.asDecimal
            guard !res.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(res)
        }
    }

    open func multiply(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch commonType(lhs, rhs) {
        case .int:
            let (res, overflow) = intValue(lhs).multipliedReportingOverflow(by: intValue(rhs))
            guard !overflow else { throw MathError.integerOverflow }
            return .int(res)
        case .float:
            let res = lhs.asFloat * rhs.asFloat
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .float(res)
        case .double:
            let res = lhs.asDouble * rhs.asDouble
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .double(res)
        case .decimal:
            let res = lhs.asDecimal * rhs.asDecimal
            guard !res.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(res)
        }
    }

    open func divide(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        guard !rhs.isZero else { throw MathError.divisionByZero }

        switch commonType(lhs, rhs) {
        case .int:
            let (a, b) = (intValue(lhs), intValue(rhs))
            if a == Int.min && b == -1 { throw MathError.integerOverflow }
            return a % b == 0 ? .int(a / b) : .double(Double(a) / Double(b))
        case .float:
            let res = lhs.asFloat / rhs.asFloat
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .float(res)
        case .double:
            let res = lhs.asDouble / rhs.asDouble
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .double(res)
        case .decimal:
            let res = lhs.asDecimal / rhs.asDecimal
            guard !res.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(res)
        }
    }

    open func power(_ base: NumericValue, _ exponent: NumericValue) throws -> NumericValue {
        switch commonType(base, exponent) {
        case .int:
            return try intPower(base: intValue(base), exponent: intValue(exponent))
        case .float:
            let res = Float(try doublePower(base: Double(base.asFloat), exponent: Double(exponent.asFloat)))
            guard res.isFinite else { throw MathError.floatingPointOverflow }
            return .float(res)
        case .double:
            return .double(try doublePower(base: base.asDouble, exponent: exponent.asDouble))
        case .decimal:
            return try decimalPower(base: base.asDecimal, exponent: exponent.asDecimal)
        }
    }

    // MARK: - Вспомогательные методы

    private func commonType(_ lhs: NumericValue, _ rhs: NumericValue) -> NumericType {
        switch (lhs.numericType, rhs.numericType) {
        case (.decimal, _), (_, .decimal): return .decimal
        case (.double, _), (_, .double): return .double
        case (.float, _), (_, .float): return .float
        case (.int, .int): return .int
        }
    }

    private func intValue(_ value: NumericValue) -> Int {
        guard case .int(let v) = value else { preconditionFailure("Ожидалось Int") }
        return v
    }

    private func doublePower(base: Double, exponent: Double) throws -> Double {
        if base == 0 && exponent < 0 { throw MathError.divisionByZero }
        if base < 0 && exponent.truncatingRemainder(dividingBy: 1) != 0 {
            throw MathError.invalidPowerOperation
        }
        let res = Foundation.pow(base, exponent)
        guard res.isFinite else { throw MathError.floatingPointOverflow }
        return res
    }

    private func intPower(base: Int, exponent: Int) throws -> NumericValue {
        if base == 0 && exponent < 0 { throw MathError.divisionByZero }
        if exponent == 0 { return .int(1) }
        if exponent < 0 {
            return .double(try doublePower(base: Double(base), exponent: Double(exponent)))
        }

        var result = 1
        var current = base
        var exp = exponent
        while exp > 0 {
            if exp & 1 == 1 {
                let (next, overflow) = result.multipliedReportingOverflow(by: current)
                guard !overflow else { throw MathError.integerOverflow }
                result = next
            }
            exp >>= 1
            if exp > 0 {
                let (next, overflow) = current.multipliedReportingOverflow(by: current)
                guard !overflow else { throw MathError.integerOverflow }
                current = next
            }
        }
        return .int(result)
    }

    private func decimalPower(base: Decimal, exponent: Decimal) throws -> NumericValue {
        if base == .zero && exponent < .zero { throw MathError.divisionByZero }
        if exponent == .zero { return .decimal(1) }

        var expCopy = exponent
        var rounded = Decimal()
        NSDecimalRound(&rounded, &expCopy, 0, .plain)

        if rounded == exponent, rounded >= Decimal(Int.min), rounded <= Decimal(Int.max) {
            let intExp = NSDecimalNumber(decimal: rounded).intValue
            if base == 1 { return .decimal(1) }
            if base == -1 { return .decimal(intExp % 2 == 0 ? 1 : -1) }
            if intExp == Int.min { throw MathError.floatingPointOverflow }

            var result = Decimal(1)
            var current = base
            var exp = abs(intExp)
            while exp > 0 {
                if exp & 1 == 1 {
                    result *= current
                    guard !result.isNaN else { throw MathError.floatingPointOverflow }
                }
                exp >>= 1
                if exp > 0 {
                    current *= current
                    guard !current.isNaN else { throw MathError.floatingPointOverflow }
                }
            }
            if intExp < 0 {
                let inverted = Decimal(1) / result
                guard !inverted.isNaN else { throw MathError.floatingPointOverflow }
                return .decimal(inverted)
            }
            return .decimal(result)
        }

        let res = try doublePower(
            base: NSDecimalNumber(decimal: base).doubleValue,
            exponent: NSDecimalNumber(decimal: exponent).doubleValue
        )
        let dec = Decimal(res)
        guard !dec.isNaN else { throw MathError.floatingPointOverflow }
        return .decimal(dec)
    }
}

/// Наследник базового калькулятора (демонстрирует наследование и переопределение).
public final class Calculator: BaseCalculator {
    public override var name: String { "Калькулятор (вариант 13)" }

    public override init() {
        super.init()
    }
}
