import Foundation

/// Базовый класс математического калькулятора, реализующий `MathOperationsProtocol`.
/// Поддерживает операции над `Int`, `Double`, `Float`, `Decimal`, а также смешанные комбинации типов.
open class BaseMathCalculator: MathOperationsProtocol {
    open var calculatorName: String {
        "Базовый математический калькулятор"
    }

    public init() {}

    open func add(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch resolvedNumericType(lhs, rhs) {
        case .int:
            let (a, b) = (extractInt(lhs), extractInt(rhs))
            let (result, overflow) = a.addingReportingOverflow(b)
            guard !overflow else { throw MathError.integerOverflow }
            return .int(result)
        case .float:
            let result = lhs.asFloat + rhs.asFloat
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .float(result)
        case .double:
            let result = lhs.asDouble + rhs.asDouble
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .double(result)
        case .decimal:
            let result = lhs.asDecimal + rhs.asDecimal
            guard !result.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(result)
        }
    }

    open func subtract(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch resolvedNumericType(lhs, rhs) {
        case .int:
            let (a, b) = (extractInt(lhs), extractInt(rhs))
            let (result, overflow) = a.subtractingReportingOverflow(b)
            guard !overflow else { throw MathError.integerOverflow }
            return .int(result)
        case .float:
            let result = lhs.asFloat - rhs.asFloat
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .float(result)
        case .double:
            let result = lhs.asDouble - rhs.asDouble
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .double(result)
        case .decimal:
            let result = lhs.asDecimal - rhs.asDecimal
            guard !result.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(result)
        }
    }

    open func multiply(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch resolvedNumericType(lhs, rhs) {
        case .int:
            let (a, b) = (extractInt(lhs), extractInt(rhs))
            let (result, overflow) = a.multipliedReportingOverflow(by: b)
            guard !overflow else { throw MathError.integerOverflow }
            return .int(result)
        case .float:
            let result = lhs.asFloat * rhs.asFloat
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .float(result)
        case .double:
            let result = lhs.asDouble * rhs.asDouble
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .double(result)
        case .decimal:
            let result = lhs.asDecimal * rhs.asDecimal
            guard !result.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(result)
        }
    }

    open func divide(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        guard !rhs.isZero else {
            throw MathError.divisionByZero
        }

        switch resolvedNumericType(lhs, rhs) {
        case .int:
            let (a, b) = (extractInt(lhs), extractInt(rhs))
            if a == Int.min && b == -1 {
                throw MathError.integerOverflow
            }
            // Если деление целочисленное без остатка — сохраняем Int, иначе возвращаем точный результат в Double.
            if a % b == 0 {
                return .int(a / b)
            }
            let result = Double(a) / Double(b)
            guard result.isFinite else { throw MathError.nonFiniteResult }
            return .double(result)
        case .float:
            let result = lhs.asFloat / rhs.asFloat
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .float(result)
        case .double:
            let result = lhs.asDouble / rhs.asDouble
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .double(result)
        case .decimal:
            let result = lhs.asDecimal / rhs.asDecimal
            guard !result.isNaN else { throw MathError.floatingPointOverflow }
            return .decimal(result)
        }
    }

    open func power(_ base: NumericValue, _ exponent: NumericValue) throws -> NumericValue {
        switch resolvedNumericType(base, exponent) {
        case .int:
            return try intPower(base: extractInt(base), exponent: extractInt(exponent))
        case .float:
            let b = base.asFloat
            let e = exponent.asFloat
            if b == 0 && e < 0 {
                throw MathError.divisionByZero
            }
            if b < 0 && e.truncatingRemainder(dividingBy: 1) != 0 {
                throw MathError.invalidPowerOperation(
                    reason: "Возведение отрицательного числа в дробную степень не поддерживается в множестве действительных чисел."
                )
            }
            let result = Float(Foundation.pow(Double(b), Double(e)))
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .float(result)
        case .double:
            let b = base.asDouble
            let e = exponent.asDouble
            if b == 0 && e < 0 {
                throw MathError.divisionByZero
            }
            if b < 0 && e.truncatingRemainder(dividingBy: 1) != 0 {
                throw MathError.invalidPowerOperation(
                    reason: "Возведение отрицательного числа в дробную степень не поддерживается в множестве действительных чисел."
                )
            }
            let result = Foundation.pow(b, e)
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .double(result)
        case .decimal:
            return try decimalPower(base: base.asDecimal, exponent: exponent.asDecimal)
        }
    }

    // MARK: - Приведение типов и вспомогательные вычисления

    private func resolvedNumericType(_ lhs: NumericValue, _ rhs: NumericValue) -> NumericType {
        switch (lhs.numericType, rhs.numericType) {
        case (.decimal, _), (_, .decimal):
            return .decimal
        case (.double, _), (_, .double):
            return .double
        case (.float, _), (_, .float):
            return .float
        case (.int, .int):
            return .int
        }
    }

    private func extractInt(_ value: NumericValue) -> Int {
        guard case .int(let intValue) = value else {
            preconditionFailure("Ожидалось значение типа Int")
        }
        return intValue
    }

    private func intPower(base: Int, exponent: Int) throws -> NumericValue {
        if base == 0 && exponent < 0 {
            throw MathError.divisionByZero
        }
        if exponent == 0 {
            return .int(1)
        }
        if exponent < 0 {
            let result = Foundation.pow(Double(base), Double(exponent))
            guard result.isFinite else { throw MathError.floatingPointOverflow }
            return .double(result)
        }

        // Быстрое целочисленное возведение в степень с контролем переполнения.
        var result = 1
        var currentBase = base
        var remainingExp = exponent

        while remainingExp > 0 {
            if remainingExp & 1 == 1 {
                let (nextResult, overflow) = result.multipliedReportingOverflow(by: currentBase)
                guard !overflow else { throw MathError.integerOverflow }
                result = nextResult
            }
            remainingExp >>= 1
            if remainingExp > 0 {
                let (nextBase, overflow) = currentBase.multipliedReportingOverflow(by: currentBase)
                guard !overflow else { throw MathError.integerOverflow }
                currentBase = nextBase
            }
        }
        return .int(result)
    }

    private func decimalPower(base: Decimal, exponent: Decimal) throws -> NumericValue {
        if base == .zero && exponent < .zero {
            throw MathError.divisionByZero
        }
        if exponent == .zero {
            return .decimal(Decimal(1))
        }

        // Если показатель степени — целое число в пределах Int, выполняем точное возведение в Decimal.
        var expCopy = exponent
        var rounded = Decimal()
        NSDecimalRound(&rounded, &expCopy, 0, .plain)

        if rounded == exponent,
           rounded >= Decimal(Int.min),
           rounded <= Decimal(Int.max) {
            let intExp = NSDecimalNumber(decimal: rounded).intValue
            if base == Decimal(1) {
                return .decimal(Decimal(1))
            }
            if base == Decimal(-1) {
                return .decimal(intExp % 2 == 0 ? Decimal(1) : Decimal(-1))
            }
            if intExp == Int.min {
                // Для |base| != 1 и показателя Int.min либо переполнение, либо потеря точности до нуля.
                throw MathError.floatingPointOverflow
            }

            let isNegativeExp = intExp < 0
            let absExp = isNegativeExp ? -intExp : intExp
            var result = Decimal(1)
            var currentBase = base
            var remaining = absExp

            while remaining > 0 {
                if remaining & 1 == 1 {
                    result *= currentBase
                    guard !result.isNaN else { throw MathError.floatingPointOverflow }
                }
                remaining >>= 1
                if remaining > 0 {
                    currentBase *= currentBase
                    guard !currentBase.isNaN else { throw MathError.floatingPointOverflow }
                }
            }

            if isNegativeExp {
                let inverted = Decimal(1) / result
                guard !inverted.isNaN else { throw MathError.floatingPointOverflow }
                return .decimal(inverted)
            }
            return .decimal(result)
        }

        // Дробный или сверхбольшой показатель степени вычисляем через Double.
        let b = NSDecimalNumber(decimal: base).doubleValue
        let e = NSDecimalNumber(decimal: exponent).doubleValue
        if b < 0 && e.truncatingRemainder(dividingBy: 1) != 0 {
            throw MathError.invalidPowerOperation(
                reason: "Возведение отрицательного числа в дробную степень не поддерживается в множестве действительных чисел."
            )
        }
        let doubleResult = Foundation.pow(b, e)
        guard doubleResult.isFinite else {
            throw MathError.floatingPointOverflow
        }
        let decimalResult = Decimal(doubleResult)
        guard !decimalResult.isNaN else {
            throw MathError.floatingPointOverflow
        }
        return .decimal(decimalResult)
    }
}
