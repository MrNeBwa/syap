import Testing
import InputValidation

@Suite("Валидация ввода")
struct InputValidationTests {

    @Test("Корректное натуральное число")
    func validNumber() throws {
        #expect(try parseNaturalNumber("100") == 100)
        #expect(try parseNaturalNumber("  42 \n") == 42)
    }

    @Test("Пустой ввод")
    func emptyInput() {
        #expect(throws: InputError.empty) {
            try parseNaturalNumber(nil)
        }
        #expect(throws: InputError.empty) {
            try parseNaturalNumber("   ")
        }
    }

    @Test("Не число")
    func notANumber() {
        #expect(throws: InputError.notANumber) {
            try parseNaturalNumber("abc")
        }
        #expect(throws: InputError.notANumber) {
            try parseNaturalNumber("3.5")
        }
    }

    @Test("Ноль и отрицательные числа")
    func notNatural() {
        #expect(throws: InputError.notNatural) {
            try parseNaturalNumber("0")
        }
        #expect(throws: InputError.notNatural) {
            try parseNaturalNumber("-5")
        }
    }

    @Test("Слишком большое число")
    func tooLarge() {
        #expect(throws: InputError.tooLarge) {
            try parseNaturalNumber("1000000000001")
        }
        #expect(throws: InputError.tooLarge) {
            try parseNaturalNumber("999999999999999999999999")
        }
    }
}
