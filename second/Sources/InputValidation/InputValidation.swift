import Foundation

public enum InputError
: Error, CustomStringConvertible, Equatable {
    case invalidInput
    case emptyInput
    case outOfRange
    case NonNaturalNumber
    case invalidElement(String)     
    case tooManyElements(max: Int)
    case elementOutOfRange(Int)

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
        case .invalidElement(let t):
            return "«\(t)» не является целым числом."
        case .tooManyElements(let max):
            return "Слишком много элементов (максимум \(max))."
        case .elementOutOfRange(let v):
            return "Число \(v) вне допустимого диапазона."
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

public let maxSequenceLength = 1000
public let elementRange = -1_000_000...1_000_000

public func parseIntSequence(_ raw: String) throws -> [Int] {
    let tokens = raw.split(whereSeparator: { $0.isWhitespace })   
    guard !tokens.isEmpty else { throw InputError.emptyInput }
    guard tokens.count <= maxSequenceLength else {
        throw InputError.tooManyElements(max: maxSequenceLength)
    }
    return try tokens.map { token in
        guard let value = Int(token) else { throw InputError.invalidElement(String(token)) }
        guard elementRange.contains(value) else { throw InputError.elementOutOfRange(value) }
        return value
    }
}

public func readValid<T>(prompt: String, parse: (String) throws -> T) -> T? {
    while true {
        print(prompt, terminator: " ")
        guard let line = readLine() else { return nil }   
        do { return try parse(line) }
        catch { print("Ошибка: \(error). Повторите ввод.") }
    }
}