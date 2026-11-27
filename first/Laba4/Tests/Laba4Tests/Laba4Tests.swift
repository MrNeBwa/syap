import Testing
import ValidateInput

@Suite("Laba4Tests")
struct Laba4Tests {
    /// `Int64.max + 1` — переполнение, должно определяться как «не число».
    @Test func overflowLiteralIsNotANumber() {
        let overflow = "9223372036854775808"
        let result = ValidateInput.validate(overflow)
        #expect(result != overflow)
        #expect(result == ValidateInput.notANumberMessage)
    }

    /// Ноль — допустимое неотрицательное число.
    @Test func zeroIsValid() {
        #expect(ValidateInput.validate("0") == "0")
    }

    /// `Int64.max` — допустимое большое положительное число.
    @Test func largePositiveIsValid() {
        #expect(ValidateInput.validate("9223372036854775807") == "9223372036854775807")
    }

    /// Правила можно подменять через existencial протокола.
    @Test func protocolExistentialSwappableRules() {
        let strict: any TextValidator = ValidateInput()
        let lenient: any TextValidator = EchoValidator()
        let validators: [any TextValidator] = [strict, lenient]
        #expect(validators.count == 2)
        #expect(strict.validate("abc") != "abc")
        #expect(lenient.validate("abc") == "abc")
    }

    /// Пустой ввод не должен вызывать краш.
    @Test func emptyInputDoesNotCrash() {
        #expect(ValidateInput.validate("") == ValidateInput.emptyMessage)
        #expect(ValidateInput.validate("   ") == ValidateInput.emptyMessage)
        #expect(ValidateInput.validate("\n") == ValidateInput.emptyMessage)
    }
}

/// Вспомогательный валидатор для демонстрации подмены правил.
private struct EchoValidator: TextValidator {
    func validate(_ text: String) -> String { text }
}
