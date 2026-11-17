// The Swift Programming Language
// https://docs.swift.org/swift-book
import Foundation
import Employee
import InputValidation

@main
struct Laba3 {
    static func main() {
        guard
        let name = ConsoleInput.read(prompt: "Введите имя:", parse: { try Validator.validateName($0) }),
        let jobTitle = ConsoleInput.read(prompt: "Введите должность:", parse: { try Validator.validateJobTitle($0) }),
        let experience = ConsoleInput.read(
            prompt: "Введите стаж работы в годах (\(Validator.minExperience)–\(Validator.maxExperience)):",
            parse: { try Validator.validateExperience($0) })
        else {
            print("\nВвод прерван.")
            exit(1)
        }
        do {
        let employee = try Employee(name: name, jobTitle: jobTitle, experience: experience)
        employee.printInfo()
        } catch {
            print("Ошибка: \(error.localizedDescription)")
            exit(1)
        }
    }
}
