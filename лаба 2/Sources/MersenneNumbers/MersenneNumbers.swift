import PrimeMath

/// Число Мерсенна: простое число вида 2^p − 1, где p — тоже простое.
public struct MersenneNumber: Equatable, Sendable {
    /// Показатель степени p (простое число).
    public let exponent: Int

    /// Само число 2^p − 1.
    public let value: Int

    public init(exponent: Int, value: Int) {
        self.exponent = exponent
        self.value = valued
    }
}

/// Позволяет красиво печатать число Мерсенна: «2^5 − 1 = 31».
extension MersenneNumber: CustomStringConvertible {
    public var description: String {
        "2^\(exponent) − 1 = \(value)"
    }
}

/// Находит все числа Мерсенна, которые строго меньше `n`.
///
/// - Parameter n: верхняя граница (натуральное число).
/// - Returns: массив найденных чисел Мерсенна в порядке возрастания.
public func findMersenneNumbers(below n: Int) -> [MersenneNumber] {
    // TODO: реализовать поиск.
    //
    // План:
    // 1. Перебирать показатель степени p, начиная с 2.
    // 2. Вычислять candidate = (1 << p) - 1.
    // 3. Останавливаться, когда candidate >= n.
    // 4. Добавлять число, если isPrime(p) && isPrime(candidate).
    //
    // Не забыть: p < 63 — защита от переполнения Int при сдвиге.
    //
    // Пока заглушка: результат всегда пуст.
    _ = n
    return []
}
