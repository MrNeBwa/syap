import Foundation

/// Наследник базового калькулятора с журналированием всех успешных вычислений.
/// Демонстрирует наследование класса (`BaseMathCalculator`), переопределение (`override`) с вызовом `super`
/// и реализацию расширенного протокола (`CalculationHistoryProtocol`).
public final class TrackingMathCalculator: BaseMathCalculator, CalculationHistoryProtocol {
    private var records: [CalculationRecord] = []

    public override var calculatorName: String {
        "Калькулятор с историей вычислений"
    }

    public var history: [CalculationRecord] {
        records
    }

    public override init() {
        super.init()
    }

    public func clearHistory() {
        records.removeAll()
    }

    public override func add(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        let result = try super.add(lhs, rhs)
        records.append(CalculationRecord(lhs: lhs, operation: .add, rhs: rhs, result: result))
        return result
    }

    public override func subtract(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        let result = try super.subtract(lhs, rhs)
        records.append(CalculationRecord(lhs: lhs, operation: .subtract, rhs: rhs, result: result))
        return result
    }

    public override func multiply(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        let result = try super.multiply(lhs, rhs)
        records.append(CalculationRecord(lhs: lhs, operation: .multiply, rhs: rhs, result: result))
        return result
    }

    public override func divide(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        let result = try super.divide(lhs, rhs)
        records.append(CalculationRecord(lhs: lhs, operation: .divide, rhs: rhs, result: result))
        return result
    }

    public override func power(_ base: NumericValue, _ exponent: NumericValue) throws -> NumericValue {
        let result = try super.power(base, exponent)
        records.append(CalculationRecord(lhs: base, operation: .power, rhs: exponent, result: result))
        return result
    }
}
