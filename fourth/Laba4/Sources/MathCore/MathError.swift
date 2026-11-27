import Foundation

/// Ошибки выполнения математических операций.
public enum MathError: Error, LocalizedError, Equatable, Sendable {
    /// Деление на ноль или возведение нуля в отрицательную степень.
    case divisionByZero
    /// Целочисленное переполнение при работе с типом `Int`.
    case integerOverflow
    /// Переполнение вещественного или десятичного типа.
    case floatingPointOverflow
    /// Математически некорректная операция возведения в степень.
    case invalidPowerOperation(reason: String)
    /// Результат вычисления не является конечным числом (NaN / Infinity).
    case nonFiniteResult

    public var errorDescription: String? {
        switch self {
        case .divisionByZero:
            return "Деление на ноль недопустимо."
        case .integerOverflow:
            return "Произошло переполнение целочисленного типа Int."
        case .floatingPointOverflow:
            return "Произошло переполнение числового типа."
        case .invalidPowerOperation(let reason):
            return reason
        case .nonFiniteResult:
            return "Результат операции не является конечным числом."
        }
    }
}
