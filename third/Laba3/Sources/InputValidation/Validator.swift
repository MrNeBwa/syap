import Foundation

public enum ValidationError: Error, Equatable, LocalizedError {
    case empty(field: String)
    case tooLong(field: String, max: Int)
    case invalidCharacters(field: String)
    case notANumber(field: String)
    case outOfRange(field: String, min: Int, max: Int)

    public var errorDescription: String? {
        switch self {
        case .empty(let f):
            return "Поле «\(f)» не может быть пустым."
        case .tooLong(let f, let max):
            return "Поле «\(f)» слишком длинное (максимум \(max) символов)."
        case .invalidCharacters(let f):
            return "Поле «\(f)» содержит недопустимые символы."
        case .notANumber(let f):
            return "Поле «\(f)» должно быть целым числом."
        case .outOfRange(let f, let min, let max):
            return "Поле «\(f)» должно быть в диапазоне от \(min) до \(max)."
        }
    }
}

public enum Validator {
    public static let maxTextLength = 100
    public static let minExperience = 0
    public static let maxExperience = 60

    public static func validateName(_ raw: String, field: String = "Имя") throws -> String {
        let text = raw.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !text.isEmpty else { throw ValidationError.empty(field: field) }
        guard text.count <= maxTextLength else {
            throw ValidationError.tooLong(field: field, max: maxTextLength)
        }
        let allowed = text.allSatisfy { $0.isLetter || $0 == " " || $0 == "-" }
        guard allowed, text.contains(where: { $0.isLetter }) else {
            throw ValidationError.invalidCharacters(field: field)
        }
        return text
    }

    public static func validateJobTitle(_ raw: String, field: String = "Должность") throws -> String {
        let text = raw.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !text.isEmpty else { throw ValidationError.empty(field: field) }
        guard text.count <= maxTextLength else {
            throw ValidationError.tooLong(field: field, max: maxTextLength)
        }
        let allowed = text.allSatisfy { $0.isLetter || $0.isNumber || " -.,".contains($0) }
        guard allowed, text.contains(where: { $0.isLetter }) else {
            throw ValidationError.invalidCharacters(field: field)
        }
        return text
    }

    public static func validateExperience(_ raw: String, field: String = "Стаж") throws -> Int {
        let text = raw.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !text.isEmpty else { throw ValidationError.empty(field: field) }
        guard let value = Int(text) else { throw ValidationError.notANumber(field: field) }
        return try validateExperience(value, field: field)
    }

    @discardableResult
    public static func validateExperience(_ value: Int, field: String = "Стаж") throws -> Int {
        guard (minExperience...maxExperience).contains(value) else {
            throw ValidationError.outOfRange(field: field, min: minExperience, max: maxExperience)
        }
        return value
    }
}
