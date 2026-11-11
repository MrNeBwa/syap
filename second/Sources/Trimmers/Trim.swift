import Foundation

public func trim(_ input: [Int]) -> [Int] {
    guard let first = input.first else { return [] }

    if first % 2 == 0 {
        return [first]                    // оставить только чётное
    } else {
        return Array(input.prefix(4))     // нечётное + 3 после = 4 элемента
    }
}

public func trim(_ input: String) -> String{
    if input.isEmpty {
        return ""
    }
    let firstChar = input.first!
    if firstChar.wholeNumberValue! % 2 == 0 {
        return String(firstChar)          // оставить только чётное
    } else {
        let endIndex = input.index(input.startIndex, offsetBy: min(4, input.count))
        return String(input[input.startIndex..<endIndex]) // нечётное + 3 после = 4 символа
    }
}