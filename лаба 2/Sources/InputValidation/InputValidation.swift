import Foundation

/// Максимально допустимое значение n.
/// Ограничение нужно, чтобы проверка на простоту выполнялась быстро.
public let maxAllowedN = 1_000_000_000_000

/// Возможные ошибки ввода.
public enum InputError: Error, CustomStringConvertible, Equatable {
    /// Пустая строка или конец ввода.
    case empty

    /// Введены не цифры (буквы, дробь и т. д.).
    case notANumber

    /// Число ноль или отрицательное.
    case notNatural

    /// Число превышает допустимый максимум.
    case tooLarge

    /// Текст сообщения об ошибке для пользователя.
    public var description: String {
        switch self {
        case .empty:
            return "ввод пуст. Введите натуральное число."
        case .notANumber:
            return "введено не целое число. Используйте только цифры."
        case .notNatural:
            return "число должно быть натуральным (больше нуля)."
        case .tooLarge:
            return "число слишком большое (максимум \(maxAllowedN))."
        }
    }
}

/// Преобразует введённую строку в натуральное число.
/// Если строка некорректна — выбрасывает ошибку `InputError`.
///
/// - Parameter raw: строка, введённая пользователем (может быть `nil`).
/// - Returns: корректное натуральное число.
public func parseNaturalNumber(_ raw: String?) throws -> Int {
    // 1. Ввод отсутствует (nil) — например, нажали Ctrl+D.
    guard let raw else { throw InputError.empty }

    // 2. Убираем пробелы и переводы строк по краям.
    let text = raw.trimmingCharacters(in: .whitespacesAndNewlines)

    // 3. Пустая строка.
    if text.isEmpty { throw InputError.empty }

    // 4. Пробуем преобразовать строку в Int.
    guard let value = Int(text) else {
        // Преобразование не удалось. Возможно, это очень длинное число:
        // отличим его от «не числа».
        var body = Substring(text)
        let isNegative = body.first == "-"
        if let first = body.first, first == "-" || first == "+" {
            body = body.dropFirst()
        }
        let onlyDigits = !body.isEmpty && body.allSatisfy { $0.isASCII && $0.isNumber }

        if onlyDigits {
            throw isNegative ? InputError.notNatural : InputError.tooLarge
        }
        throw InputError.notANumber
    }

    // 5. Натуральное число — строго больше нуля.
    guard value > 0 else { throw InputError.notNatural }

    // 6. Проверка верхней границы.
    guard value <= maxAllowedN else { throw InputError.tooLarge }

    return value
}
