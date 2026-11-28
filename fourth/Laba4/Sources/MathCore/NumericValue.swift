import Foundation

/// Поддерживаемые числовые типы.
public enum NumericType: Int, CaseIterable, Equatable, CustomStringConvertible, Sendable {
    case int = 1
    case double = 2
    case float = 3
    case decimal = 4

    public var description: String {
        switch self {
        case .int: return "Int"
        case .double: return "Double"
        case .float: return "Float"
        case .decimal: return "Decimal"
        }
    }
}

/// Числовое значение одного из поддерживаемых типов.
public enum NumericValue: Equatable, CustomStringConvertible, Sendable {
    case int(Int)
    case double(Double)
    case float(Float)
    case decimal(Decimal)

    public var numericType: NumericType {
        switch self {
        case .int: return .int
        case .double: return .double
        case .float: return .float
        case .decimal: return .decimal
        }
    }

    public var isZero: Bool {
        switch self {
        case .int(let v): return v == 0
        case .double(let v): return v == 0
        case .float(let v): return v == 0
        case .decimal(let v): return v == .zero
        }
    }

    public var asDouble: Double {
        switch self {
        case .int(let v): return Double(v)
        case .double(let v): return v
        case .float(let v): return Double(v)
        case .decimal(let v): return NSDecimalNumber(decimal: v).doubleValue
        }
    }

    public var asFloat: Float {
        switch self {
        case .int(let v): return Float(v)
        case .double(let v): return Float(v)
        case .float(let v): return v
        case .decimal(let v): return NSDecimalNumber(decimal: v).floatValue
        }
    }

    public var asDecimal: Decimal {
        switch self {
        case .int(let v): return Decimal(v)
        case .double(let v): return Decimal(v)
        case .float(let v): return Decimal(Double(v))
        case .decimal(let v): return v
        }
    }

    public var description: String {
        switch self {
        case .int(let v): return "\(v) (Int)"
        case .double(let v): return "\(v) (Double)"
        case .float(let v): return "\(v) (Float)"
        case .decimal(let v): return "\(NSDecimalNumber(decimal: v).stringValue) (Decimal)"
        }
    }
}

/// Доступные математические операции.
public enum MathOperation: Int, CaseIterable, Equatable, CustomStringConvertible, Sendable {
    case add = 1
    case subtract = 2
    case multiply = 3
    case divide = 4
    case power = 5

    public var symbol: String {
        switch self {
        case .add: return "+"
        case .subtract: return "-"
        case .multiply: return "*"
        case .divide: return "/"
        case .power: return "^"
        }
    }

    public var description: String {
        switch self {
        case .add: return "Сложение (+)"
        case .subtract: return "Вычитание (-)"
        case .multiply: return "Умножение (*)"
        case .divide: return "Деление (/)"
        case .power: return "Возведение в степень (^)"
        }
    }
}

/// Ошибки при выполнении математических операций.
public enum MathError: Error, LocalizedError, Equatable, Sendable {
    case divisionByZero
    case integerOverflow
    case floatingPointOverflow
    case invalidPowerOperation

    public var errorDescription: String? {
        switch self {
        case .divisionByZero:
            return "Деление на ноль недопустимо."
        case .integerOverflow:
            return "Переполнение целочисленного типа Int."
        case .floatingPointOverflow:
            return "Переполнение числового типа."
        case .invalidPowerOperation:
            return "Невозможно возвести отрицательное число в дробную степень."
        }
    }
}
