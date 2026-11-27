import Foundation

/// Ошибки валидации пользовательского ввода.
public enum ValidationError: Error, LocalizedError, Equatable, Sendable {
    /// Пользователь ввёл пустую строку или только пробельные символы.
    case emptyInput
    /// Выбранный пункт меню не попадает в допустимый диапазон.
    case invalidMenuChoice(validRange: ClosedRange<Int>)
    /// Строка не соответствует синтаксису указанного числового типа.
    case invalidNumberFormat(typeName: String)
    /// Введено нечисловое или бесконечное значение (NaN, Inf).
    case nonFiniteNumber
    /// Число выходит за допустимые пределы выбранного типа.
    case numberOutOfRange(typeName: String)

    public var errorDescription: String? {
        switch self {
        case .emptyInput:
            return "Ввод не может быть пустым."
        case .invalidMenuChoice(let validRange):
            return "Некорректный пункт меню. Введите целое число от \(validRange.lowerBound) до \(validRange.upperBound)."
        case .invalidNumberFormat(let typeName):
            return "Некорректный формат числа для типа \(typeName)."
        case .nonFiniteNumber:
            return "Значение должно быть конечным числом (NaN и Infinity не допускаются)."
        case .numberOutOfRange(let typeName):
            return "Введённое число выходит за допустимый диапазон для типа \(typeName)."
        }
    }
}
