import Foundation

/// Перечень математических операций, поддерживаемых калькулятором.
public enum MathOperation: Int, CaseIterable, Equatable, CustomStringConvertible, Sendable {
    case add = 1
    case subtract = 2
    case multiply = 3
    case divide = 4
    case power = 5

    /// Математический символ операции.
    public var symbol: String {
        switch self {
        case .add:
            return "+"
        case .subtract:
            return "-"
        case .multiply:
            return "*"
        case .divide:
            return "/"
        case .power:
            return "^"
        }
    }

    /// Русскоязычное название операции.
    public var title: String {
        switch self {
        case .add:
            return "Сложение"
        case .subtract:
            return "Вычитание"
        case .multiply:
            return "Умножение"
        case .divide:
            return "Деление"
        case .power:
            return "Возведение в степень"
        }
    }

    public var description: String {
        "\(title) (\(symbol))"
    }
}
