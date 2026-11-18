import Foundation

/// Правило валидации текстового ввода.
public protocol TextValidator {
    /// Возвращает `text` без изменений, если он корректен,
    /// иначе — сообщение об ошибке.
    func validate(_ text: String) -> String
}

/// Валидатор целых неотрицательных чисел в диапазоне `0...Int64.max`.
public struct ValidateInput: TextValidator {
    /// Допустимый диапазон (включая ноль).
    public static let range: ClosedRange<Int64> = 0...Int64.max

    /// Сообщение об ошибке для пустого или только-пробельного ввода.
    public static let emptyMessage = "ввод пуст. Введите число."

    /// Сообщение об ошибке, если ввод не является допустимым числом
    /// (не цифры, переполнение `Int64` или отрицательное число).
    public static let notANumberMessage = "введено не целое число"

    public init() {}

    public func validate(_ text: String) -> String {
        let trimmed = text.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !trimmed.isEmpty else { return Self.emptyMessage }
        guard let value = Int64(trimmed), value >= 0 else {
            return Self.notANumberMessage
        }
        return trimmed
    }

    // Точка входа на уровне типа, чтобы `main()` мог вызывать
    // `ValidateInput.validate(...)` без создания экземпляра.
    public static func validate(_ text: String) -> String {
        ValidateInput().validate(text)
    }
}
