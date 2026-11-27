import Foundation

/// Поддерживаемые числовые типы для выполнения математических операций.
public enum NumericType: Int, CaseIterable, Equatable, CustomStringConvertible, Sendable {
    case int = 1
    case double = 2
    case float = 3
    case decimal = 4

    public var description: String {
        switch self {
        case .int:
            return "Int (целое число)"
        case .double:
            return "Double (вещественное двойной точности)"
        case .float:
            return "Float (вещественное одинарной точности)"
        case .decimal:
            return "Decimal (десятичное высокой точности)"
        }
    }
}
