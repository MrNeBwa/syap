import Testing
import PrimeMath
import MersenneNumbers

// Тесты ниже включатся, когда будут реализованы функции-заглушки.
// Уберите `.disabled(...)`, чтобы они начали выполняться.

@Suite("Проверка на простоту")
struct PrimeMathTests {

    @Test("Простые числа", .disabled("TODO: реализуйте isPrime"))
    func primes() {
        #expect(isPrime(2))
        #expect(isPrime(3))
        #expect(isPrime(31))
        #expect(isPrime(97))
    }

    @Test("Составные числа", .disabled("TODO: реализуйте isPrime"))
    func composites() {
        #expect(!isPrime(0))
        #expect(!isPrime(1))
        #expect(!isPrime(4))
        #expect(!isPrime(100))
        #expect(!isPrime(-7))
    }
}

@Suite("Числа Мерсенна")
struct MersenneNumbersTests {

    @Test("Числа, меньшие 100", .disabled("TODO: реализуйте findMersenneNumbers"))
    func below100() {
        let values = findMersenneNumbers(below: 100).map(\.value)
        #expect(values == [3, 7, 31])
    }

    @Test("Граница: n = 3", .disabled("TODO: реализуйте findMersenneNumbers"))
    func boundary() {
        #expect(findMersenneNumbers(below: 3).isEmpty)
        #expect(findMersenneNumbers(below: 4).map(\.value) == [3])
    }
}
