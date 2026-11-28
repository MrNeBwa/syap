import Foundation

public enum ValidationError: Error, LocalizedError, Equatable, Sendable {
    case emptyInput
    case invalidMenuChoice(ClosedRange<Int>)
    case invalidNumberFormat(String)
    case nonFiniteNumber
    case numberOutOfRange(String)

    public var errorDescription: String? {
        switch self {
        case .emptyInput:
            return "Ввод не может быть пустым."
        case .invalidMenuChoice(let range):
            return "Введите целое число от \(range.lowerBound) до \(range.upperBound)."
        case .invalidNumberFormat(let typeName):
            return "Некорректный формат для типа \(typeName)."
        case .nonFiniteNumber:
            return "Значение должно быть конечным числом."
        case .numberOutOfRange(let typeName):
            return "Число выходит за пределы типа \(typeName)."
        }
    }
}

public enum Validator {
    public static func validateMenuChoice(_ raw: String, in range: ClosedRange<Int>) throws -> Int {
        let text = try nonEmpty(raw)
        guard matches(text, pattern: #"^[+-]?\d+$"#),
              let value = Int(text),
              range.contains(value) else {
            throw ValidationError.invalidMenuChoice(range)
        }
        return value
    }

    public static func validateInt(_ raw: String) throws -> Int {
        let text = try nonEmpty(raw)
        guard matches(text, pattern: #"^[+-]?\d+$"#) else {
            throw ValidationError.invalidNumberFormat("Int")
        }
        guard let value = Int(text) else {
            throw ValidationError.numberOutOfRange("Int")
        }
        return value
    }

    public static func validateDouble(_ raw: String) throws -> Double {
        let text = try normalizedFloat(raw, typeName: "Double")
        guard let value = Double(text) else {
            throw ValidationError.invalidNumberFormat("Double")
        }
        guard value.isFinite else {
            throw ValidationError.numberOutOfRange("Double")
        }
        return value
    }

    public static func validateFloat(_ raw: String) throws -> Float {
        let text = try normalizedFloat(raw, typeName: "Float")
        guard let value = Float(text) else {
            throw ValidationError.invalidNumberFormat("Float")
        }
        guard value.isFinite else {
            throw ValidationError.numberOutOfRange("Float")
        }
        return value
    }

    public static func validateDecimal(_ raw: String) throws -> Decimal {
        let text = try normalizedFloat(raw, typeName: "Decimal")
        guard let value = Decimal(string: text, locale: Locale(identifier: "en_US_POSIX")) else {
            throw ValidationError.invalidNumberFormat("Decimal")
        }
        guard !value.isNaN else {
            throw ValidationError.numberOutOfRange("Decimal")
        }
        return value
    }


    private static func nonEmpty(_ raw: String) throws -> String {
        let trimmed = raw.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !trimmed.isEmpty else { throw ValidationError.emptyInput }
        return trimmed
    }

    private static func normalizedFloat(_ raw: String, typeName: String) throws -> String {
        let trimmed = try nonEmpty(raw)
        let lower = trimmed.lowercased()
        if ["nan", "+nan", "-nan", "inf", "+inf", "-inf", "infinity", "+infinity", "-infinity"].contains(lower) {
            throw ValidationError.nonFiniteNumber
        }
        let text = (trimmed.contains(",") && !trimmed.contains("."))
            ? trimmed.replacingOccurrences(of: ",", with: ".")
            : trimmed
        guard matches(text, pattern: #"^[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?$"#) else {
            throw ValidationError.invalidNumberFormat(typeName)
        }
        return text
    }

    private static func matches(_ text: String, pattern: String) -> Bool {
        text.range(of: pattern, options: .regularExpression) != nil
    }
}
