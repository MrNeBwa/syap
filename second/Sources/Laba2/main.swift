import Foundation
import InputValidation
import MerseneNums


func readValidNumber() -> Int {
    while True {
        print("Enter real number n: ", terminator: "")
        let line = readLine()
        guard let line else {
            print("Input is corrupted")
            return nil
        }
        do {
            return try parseNaturalNumber()
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


print(" === Laba2 ===")




print("End of program")

