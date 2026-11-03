import Foundation
import InputValidation
import MersenneNumbers

func readValidN() -> Int? {
    while true {
        print("Введите натуральное число n: ", terminator: "")
        let line = readLine()
        guard let line else {
            print("\nВвод завершён. Выход из программы.")
            return nil
        }

        do {
            return try parseNaturalNumber(line)
        } catch let error as InputError {
            print("Ошибка: \(error)")
        } catch {
            print("Неизвестная ошибка: \(error)")
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

print("=== Лабораторная работа № 2 ===")

repeat {
    guard let n = readValidN() else { break }
    let numbers = findMersenneNumbers(below: n)
    if numbers.isEmpty {
        print("Чисел Мерсенна, меньших \(n), не найдено.")
    } else {
        print("Числа Мерсенна, меньшие \(n):")
        for number in numbers {
            print("  \(number)") // использует description
        }
        print("Найдено чисел: \(numbers.count)")
    }
} while askToRepeat()

print("Работа завершена.")
