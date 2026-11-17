public protocol Math<T> {
    associatedtype T
    func add(_ a: T, _ b: T) -> T
    func subtract(_ a: T, _ b: T) -> T
    func multiply(_ a: T, _ b: T) -> T
    func divide(_ a: T, _ b: T) -> T
}

public extension Math where T: Numeric {
    func add(_ a: T, _ b: T) -> T { a + b }
    func subtract(_ a: T, _ b: T) -> T { a - b }
    func multiply(_ a: T, _ b: T) -> T { a * b }
}

public extension Math where T: BinaryInteger {
    func divide(_ a: T, _ b: T) -> T { a / b }       // целочисленное деление
}

public extension Math where T: FloatingPoint {
    func divide(_ a: T, _ b: T) -> T { a / b }
}

public struct IntMath: Math { 
    public typealias T = Int 
    public init() {}
}
public struct DoubleMath: Math { 
    public typealias T = Double
    public init() {}
}
