// swift-tools-version: 6.4
import PackageDescription

let package = Package(
    name: "Laba2",
    targets: [
        .target (name: "InputValidation"),

        .target (name: "MerseneNums", dependencies: ["InputValidation"]),

        .executableTarget(
            name: "Laba2",
            dependencies: ["InputValidation", "MerseneNums"]
        )
    ]
)