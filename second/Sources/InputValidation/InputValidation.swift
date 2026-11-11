import Foundation



public enum InputError
: Error, CustomStringConvertible, Equatable {
    case invalidInput
    case emptyInput
    case outOfRange
    case NonNaturalNumber

    public var description: String {
        switch self {
        case .invalidInput:
            return "Invalid input. Please enter a valid number."
        case .emptyInput:
            return "Input is empty. Please enter a number."
        case .outOfRange:
            return "Input is out of range. Please enter a number within the valid range."
        case .NonNaturalNumber:
            return "Input is not a natural number. Please enter a valid natural number."
        }
    }
}

public func parseNaturalNumber(_ raw: String) throws -> Int {
    guard !raw.isEmpty else {
        throw InputError.emptyInput
    }
    
    guard let number = Int(raw) else {
        throw InputError.invalidInput
    }
    
    guard number >= 0 else {
        throw InputError.NonNaturalNumber
    }
    
    return number
}