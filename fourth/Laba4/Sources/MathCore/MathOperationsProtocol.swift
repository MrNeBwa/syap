import Foundation

/// Протокол для выполнения математических операций над различными числовыми типами.
public protocol MathOperationsProtocol: AnyObject {
    /// Название реализации калькулятора.
    var calculatorName: String { get }

    /// Сложение двух числовых значений.
    func add(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    /// Вычитание правого операнда из левого.
    func subtract(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    /// Умножение двух числовых значений.
    func multiply(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    /// Деление левого операнда на правый.
    func divide(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    /// Возведение основания в степень.
    func power(_ base: NumericValue, _ exponent: NumericValue) throws -> NumericValue
}

/// Расширение протокола с реализацией по умолчанию для универсального вызова операции по `MathOperation`.
public extension MathOperationsProtocol {
    func execute(_ operation: MathOperation, _ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch operation {
        case .add:
            return try add(lhs, rhs)
        case .subtract:
            return try subtract(lhs, rhs)
        case .multiply:
            return try multiply(lhs, rhs)
        case .divide:
            return try divide(lhs, rhs)
        case .power:
            return try power(lhs, rhs)
        }
    }
}

/// Запись об одной выполненной математической операции.
public struct CalculationRecord: Equatable, CustomStringConvertible, Sendable {
    public let lhs: NumericValue
    public let operation: MathOperation
    public let rhs: NumericValue
    public let result: NumericValue

    public init(lhs: NumericValue, operation: MathOperation, rhs: NumericValue, result: NumericValue) {
        self.lhs = lhs
        self.operation = operation
        self.rhs = rhs
        self.result = result
    }

    public var description: String {
        "\(lhs.description) \(operation.symbol) \(rhs.description) = \(result.description)"
    }
}

/// Протокол калькулятора с поддержкой истории вычислений (наследует `MathOperationsProtocol`).
public protocol CalculationHistoryProtocol: MathOperationsProtocol {
    /// Список выполненных операций в порядке их вычисления.
    var history: [CalculationRecord] { get }
    /// Очищает накопленную историю вычислений.
    func clearHistory()
}
