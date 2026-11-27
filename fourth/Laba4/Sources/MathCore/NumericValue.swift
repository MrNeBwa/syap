import Foundation

/// Универсальное числовое значение, объединяющее поддерживаемые числовые типы Swift.
public enum NumericValue: Equatable, CustomStringConvertible, Sendable {
    case int(Int)
    case double(Double)
    case float(Float)
    case decimal(Decimal)

    /// Тип хранимого числа.
    public var numericType: NumericType {
        switch self {
        case .int:
            return .int
        case .double:
            return .double
        case .float:
            return .float
        case .decimal:
            return .decimal
        }
    }

    /// Проверяет, равно ли значение нулю.
    public var isZero: Bool {
        switch self {
        case .int(let value):
            return value == 0
        case .double(let value):
            return value == 0
        case .float(let value):
            return value == 0
        case .decimal(let value):
            return value == .zero
        }
    }

    /// Преобразование значения к `Double`.
    public var asDouble: Double {
        switch self {
        case .int(let value):
            return Double(value)
        case .double(let value):
            return value
        case .float(let value):
            return Double(value)
        case .decimal(let value):
            return NSDecimalNumber(decimal: value).doubleValue
        }
    }

    /// Преобразование значения к `Float`.
    public var asFloat: Float {
        switch self {
        case .int(let value):
            return Float(value)
        case .double(let value):
            return Float(value)
        case .float(let value):
            return value
        case .decimal(let value):
            return NSDecimalNumber(decimal: value).floatValue
        }
    }

    /// Преобразование значения к `Decimal`.
    public var asDecimal: Decimal {
        switch self {
        case .int(let value):
            return Decimal(value)
        case .double(let value):
            return Decimal(value)
        case .float(let value):
            return Decimal(Double(value))
        case .decimal(let value):
            return value
        }
    }

    /// Форматированное значение без указания типа.
    public var formattedValue: String {
        switch self {
        case .int(let value):
            return "\(value)"
        case .double(let value):
            return "\(value)"
        case .float(let value):
            return "\(value)"
        case .decimal(let value):
            return NSDecimalNumber(decimal: value).stringValue
        }
    }

    public var description: String {
        switch self {
        case .int:
            return "\(formattedValue) (Int)"
        case .double:
            return "\(formattedValue) (Double)"
        case .float:
            return "\(formattedValue) (Float)"
        case .decimal:
            return "\(formattedValue) (Decimal)"
        }
    }
}
