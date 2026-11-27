import Foundation

/// Класс валидации пользовательского ввода, реализующий `InputValidatorProtocol`.
public final class InputValidator: InputValidatorProtocol {
    public init() {}

    public func validateMenuChoice(_ raw: String, in range: ClosedRange<Int>) throws -> Int {
        let trimmed = try requireNonEmpty(raw)
        guard isValidIntegerSyntax(trimmed) else {
            throw ValidationError.invalidMenuChoice(validRange: range)
        }
        guard let value = Int(trimmed), range.contains(value) else {
            throw ValidationError.invalidMenuChoice(validRange: range)
        }
        return value
    }

    public func validateInt(_ raw: String) throws -> Int {
        let trimmed = try requireNonEmpty(raw)
        guard isValidIntegerSyntax(trimmed) else {
            throw ValidationError.invalidNumberFormat(typeName: "Int")
        }
        guard let value = Int(trimmed) else {
            throw ValidationError.numberOutOfRange(typeName: "Int")
        }
        return value
    }

    public func validateDouble(_ raw: String) throws -> Double {
        let normalized = try normalizeFloatingPointInput(raw, typeName: "Double")
        guard let value = Double(normalized) else {
            throw ValidationError.invalidNumberFormat(typeName: "Double")
        }
        guard value.isFinite else {
            throw ValidationError.numberOutOfRange(typeName: "Double")
        }
        return value
    }

    public func validateFloat(_ raw: String) throws -> Float {
        let normalized = try normalizeFloatingPointInput(raw, typeName: "Float")
        guard let value = Float(normalized) else {
            throw ValidationError.invalidNumberFormat(typeName: "Float")
        }
        guard value.isFinite else {
            throw ValidationError.numberOutOfRange(typeName: "Float")
        }
        return value
    }

    public func validateDecimal(_ raw: String) throws -> Decimal {
        let normalized = try normalizeFloatingPointInput(raw, typeName: "Decimal")
        // Предварительная проверка синтаксиса защищает от частичного разбора строк вроде "12abc" в Decimal(string:).
        guard let value = Decimal(string: normalized, locale: Locale(identifier: "en_US_POSIX")) else {
            throw ValidationError.invalidNumberFormat(typeName: "Decimal")
        }
        guard !value.isNaN else {
            throw ValidationError.numberOutOfRange(typeName: "Decimal")
        }
        return value
    }

    // MARK: - Вспомогательные методы проверки

    private func requireNonEmpty(_ raw: String) throws -> String {
        let trimmed = raw.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !trimmed.isEmpty else {
            throw ValidationError.emptyInput
        }
        return trimmed
    }

    private func normalizeFloatingPointInput(_ raw: String, typeName: String) throws -> String {
        let trimmed = try requireNonEmpty(raw)
        let lower = trimmed.lowercased()
        if lower == "nan" || lower == "+nan" || lower == "-nan" ||
            lower == "inf" || lower == "+inf" || lower == "-inf" ||
            lower == "infinity" || lower == "+infinity" || lower == "-infinity" {
            throw ValidationError.nonFiniteNumber
        }

        // Разрешаем одиночную запятую в качестве десятичного разделителя (удобно для русской раскладки),
        // если точка не используется одновременно с ней.
        let normalized: String
        if trimmed.contains(",") && !trimmed.contains(".") {
            normalized = trimmed.replacingOccurrences(of: ",", with: ".")
        } else {
            normalized = trimmed
        }

        guard isValidFloatingSyntax(normalized) else {
            throw ValidationError.invalidNumberFormat(typeName: typeName)
        }
        return normalized
    }

    private func isValidIntegerSyntax(_ text: String) -> Bool {
        let pattern = #"^[+-]?\d+$"#
        return text.range(of: pattern, options: .regularExpression) != nil
    }

    private func isValidFloatingSyntax(_ text: String) -> Bool {
        // Допускает необязательный знак, целую и/или дробную часть (но минимум одну цифру) и необязательную экспоненту.
        let pattern = #"^[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?$"#
        return text.range(of: pattern, options: .regularExpression) != nil
    }
}
