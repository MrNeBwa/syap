// swift-tools-version: 6.4
// Минимальная версия Swift, необходимая для сборки пакета.

import PackageDescription

let package = Package(
    name: "Laba4",
    targets: [
        // Пакет (модуль) валидации пользовательского ввода и работы с консолью.
        .target(name: "InputValidation"),
        // Пакет (модуль) математических протоколов, типов чисел и калькуляторов.
        .target(name: "MathCore"),
        // Исполняемый модуль приложения, работающий через экземпляры протоколов.
        .executableTarget(
            name: "Laba4",
            dependencies: ["MathCore", "InputValidation"],
            swiftSettings: [
                .enableUpcomingFeature("ApproachableConcurrency"),
            ]
        ),
        // Модуль модульных тестов.
        .testTarget(
            name: "Laba4Tests",
            dependencies: ["MathCore", "InputValidation"],
            swiftSettings: [
                .enableUpcomingFeature("ApproachableConcurrency"),
            ]
        ),
    ]
)
