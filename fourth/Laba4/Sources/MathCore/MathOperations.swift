import Foundation

public protocol MathOperations: AnyObject {
    var name: String { get }

    func add(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    func subtract(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    func multiply(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    func divide(_ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue
    func power(_ base: NumericValue, _ exponent: NumericValue) throws -> NumericValue
}

public extension MathOperations {
    func execute(_ operation: MathOperation, _ lhs: NumericValue, _ rhs: NumericValue) throws -> NumericValue {
        switch operation {
        case .add: return try add(lhs, rhs)
        case .subtract: return try subtract(lhs, rhs)
        case .multiply: return try multiply(lhs, rhs)
        case .divide: return try divide(lhs, rhs)
        case .power: return try power(lhs, rhs)
        }
    }
}
