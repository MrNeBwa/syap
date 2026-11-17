import XCTest
import InputValidation
@testable import Employee

final class EmployeeModelTests: XCTestCase {
    func testYearsWord() {
        let expected: [Int: String] = [
            0: "лет", 1: "год", 2: "года", 3: "года", 4: "года", 5: "лет",
            9: "лет", 10: "лет", 11: "лет", 12: "лет", 13: "лет", 14: "лет",
            15: "лет", 20: "лет", 21: "год", 22: "года", 25: "лет",
            31: "год", 101: "год", 111: "лет", 112: "лет", 121: "год"
        ]
        for (n, word) in expected {
            XCTAssertEqual(YearsFormatter.word(for: n), word, "для \(n)")
        }
    }

    func testInfoFormat() throws {
        let e = try Employee(name: "Василий", jobTitle: "Системный администратор", experience: 3)
        XCTAssertEqual(e.info, "Имя: Василий Должность: Системный администратор Стаж: 3 года")
    }

    func testInfoOneAndFiveYears() throws {
        XCTAssertTrue(try Employee(name: "Анна", jobTitle: "Тестировщик", experience: 1).info.hasSuffix("Стаж: 1 год"))
        XCTAssertTrue(try Employee(name: "Анна", jobTitle: "Тестировщик", experience: 5).info.hasSuffix("Стаж: 5 лет"))
    }

    func testTrimsWhitespace() throws {
        let e = try Employee(name: "  Иван  ", jobTitle: " Повар ", experience: 0)
        XCTAssertEqual(e.name, "Иван")
        XCTAssertEqual(e.jobTitle, "Повар")
    }

    func testInvalidEmployeeIsRejected() {
        XCTAssertThrowsError(try Employee(name: "", jobTitle: "Повар", experience: 1))
        XCTAssertThrowsError(try Employee(name: "Иван", jobTitle: "  ", experience: 1))
        XCTAssertThrowsError(try Employee(name: "Иван", jobTitle: "Повар", experience: -1))
        XCTAssertThrowsError(try Employee(name: "Иван", jobTitle: "Повар", experience: 61))
    }
}
