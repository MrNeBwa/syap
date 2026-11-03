// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "Lab2Mersenne",
    targets: [
        .target(name: "PrimeMath"),

        .target(name: "MersenneNumbers", dependencies: ["PrimeMath"]),

        .target(name: "InputValidation"),

        .executableTarget(
            name: "Lab2Mersenne",
            dependencies: ["MersenneNumbers", "InputValidation"]
        ),

        .testTarget(
            name: "Lab2MersenneTests",
            dependencies: ["PrimeMath", "MersenneNumbers", "InputValidation"]
        )
    ]
)
