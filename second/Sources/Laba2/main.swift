import Foundation
import InputValidation
import MerseneNums
import Trimmers

func readValidNumber() -> Int? {
    while true {
        print("Enter real number n: ", terminator: "")
        let line = readLine()
        guard let line else {
            print("Input is corrupted")
            return nil
        }
        do {
            return try parseNaturalNumber(line)
        } catch let error as InputError{
            print("Input Error: \(error)")
            
        } catch {
            print("Unknow Error: \(error)")
        }
    }
}

func askToRepeat() -> Bool {
    while true {
        print("Повторить? (y/n): ", terminator: "")
        guard let answer = readLine()?
            .trimmingCharacters(in: .whitespacesAndNewlines)
            .lowercased() else { return false }

        switch answer {
        case "д", "да", "y", "yes":
            return true
        case "н", "нет", "n", "no":
            return false
        default:
            print("Введите «y» или «n».")
        }
    }
}

print(" === Laba2 | First Task ===")

repeat {
    guard let n = readValidNumber() else {
        print("Input is corrupted")
        break
    }
    let numbers = findMersenneNumbers(below: n)
    if numbers.isEmpty {
        print("Чисел Мерсенна, меньших \(n), не найдено.")
    } else {
        print("Числа Мерсенна, меньшие \(n):")
        for number in numbers {
            print("  \(number)")
        }
        print("Найдено чисел: \(numbers.count)")
    }
} while askToRepeat()

print (" === Second Task ===")

let trimFun : ([Int]) -> [Int] = trim

repeat {
    guard let seq = InputValidation.readValid(prompt: "Введите целые числа через пробел:", parse: parseIntSequence) else { break }
    print("Результат: \(trimFun(seq))")
} while askToRepeat()


print("End of program")

