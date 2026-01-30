# Fleet-Manager

## Project Overview

A logistics management application developed in C++17 with Qt Widgets for coordinating delivery vehicles (drones, electric vans, sidewalk robots) and their cargo. The system implements resource management, capacity tracking, and operational workflows.

***In this application I tried to develop a solution that applies C++ principles for resource management, exception safety, and performance considerations while maintaining extensibility and reusability through component-based architecture.***

## System Architecture

### Core Class Hierarchy

The system uses polymorphic inheritance with abstract base classes for vehicles and packages, enabling management of type-specific behaviors while maintaining a unified interface.

#### Vehicle Management Classes
**Base Class - `Vehicle`**
- Provides functionality for capacity management, state control, and package storage
- Implements unique ID generation through static counter
- Uses const-correct interfaces for query operations that don't modify state

**Derived Vehicle Implementations**

| Class | Capacity | Special Attributes | Use Case |
|-------|----------|-------------------|----------|
| `Drone` | 5,000g (5kg) | Autonomy (kilometers) | Aerial deliveries |
| `Van` | 500,000g (500kg) | None | Ground transportation |
| `Robot` | 20,000g (20kg) | None | Urban delivery |

#### Package Management System
Specialized package types inherit from a common base class:

| Package Type | Unique Attributes | Validation |
|--------------|------------------|------------|
| `StandardPackage` | None | Weight validation |
| `FragilePackage` | Fragility levels (Low/Medium/High) | Weight validation |
| `RefrigeratedPackage` | Temperature control (-50°C to +10°C) | Weight + Temperature range validation |
| `ExpressPackage` | Priority handling (URGENT) | Weight validation |

### Central Controller: FleetManager

The controller coordinates system operations:
- Factory methods for object creation with validation
- Operations using validation-before-execution pattern to prevent invalid state changes
- Statistics calculation with incremental updates
- Compatibility checking before operations to reduce failed assignments

## Main application interface showing vehicle fleet, pending packages, and management controls
### Initial State
<br>
<img width="1363" height="705" alt="image" src="https://github.com/user-attachments/assets/ef105df5-8f65-44f5-8569-91a5718638f7" />

### Operational
<br>
<img width="1363" height="707" alt="operational2" src="https://github.com/user-attachments/assets/5600aa31-3b04-4822-b9c8-8ebaf936f487" />


## Implementation

### Memory Management

**Ownership Model**: Uses `std::unique_ptr` to establish ownership chains from fleet to vehicle to package, ensuring clear responsibility for cleanup.

**Move Semantics**: Resource transfers use move operations to avoid copy overhead when passing objects between containers.

**RAII**: Resource-owning classes implement exception guarantees. Constructors validate parameters before initialization to prevent partially constructed objects.

### Software Design Principles

**>> DRY Principle**: Centralized handlers eliminate code duplication, demonstrated by the `handlePackageCreation()` method that consolidates common logic.

**>> Single Responsibility**: Components have specific purposes: creation, business logic, presentation, and system utilities to maintain focus.

**>> Separation of Concerns**: Division between data management, business logic, and presentation enables independent development and testing.

**>> Factory Methods**: Creator classes (`PackageCreator`, `VehicleCreator`) centralize object construction with validation to ensure consistency.

**>> Command Pattern**: Operation controllers encapsulate specific business workflows, separating UI interactions from system operations.

### C++17 Features Used

- **Smart Pointers**: Uses `std::unique_ptr` with `std::make_unique` for exception-safe object construction and automatic memory management.

- **Move-Only Types**: Ownership model relies on move semantics for resource transfers between components, preventing accidental copies.

- **Structured Returns**: Dialog controllers use `std::pair` returns for clean multi-value handling without output parameters.

- **Compile-Time Constants**: Vehicle capacity limits defined as constexpr values (`5000`, `500000`, `20000`) for optimization opportunities.

- **Return Value Optimization**: Factory methods return `std::unique_ptr` by value to enable compiler optimizations and eliminate temporary copies.

### Performance Considerations

**Incremental State Tracking**: Vehicle capacity maintained as running totals updated during operations to avoid O(n) recalculations during queries.

**Early-Exit Validation**: Operations perform state checks before computations to reduce processing for invalid requests.

**Cache-Friendly Data Structures**: Uses `std::vector<std::unique_ptr<T>>` for contiguous storage and efficient iteration during UI rendering.

**Sink Parameter Design**: Methods use parameters that indicate ownership transfer, clearly communicating resource movement.

**Type Identification**: Uses `dynamic_cast` in UI components for specific type handling where runtime type information is needed for display.

### Component Architecture

**Modular Design**: Independent UI components (`StatisticsPanel`, `SystemLogWidget`, `VehicleInfoViewer`) provide reusable functionality with clear interfaces.

**Observer Pattern**: Uses Qt's signal-slot mechanism for updates between backend and frontend, maintaining loose coupling.

**Builder-like Dialogs**: Step-by-step object construction with validation at each stage ensures only valid objects enter the system.

### Design Patterns

**Facade Pattern**: `FleetManager` provides a simplified interface to subsystem interactions, hiding complexity from UI components.

**Mediator Pattern**: Components communicate through central controllers rather than direct dependencies, reducing coupling.

**Null Object Pattern**: Failed operations return `nullptr` as a clear failure indicator, avoiding complex error state objects.

## Data Integrity & Validation

### Validation Pipeline

Three-layer validation:
1. **Frontend Validation**: Dialog input validation prevents malformed data from reaching business logic
2. **Backend Validation**: Core class rule enforcement maintains domain integrity
3. **State Validation**: System state consistency checking ensures valid operations

### Validation-Before-Execution

**Precondition Checking**: Operations validate requirements before state modification to avoid partial updates.

**All-or-Nothing Semantics**: Operations either complete fully or not at all, maintaining system consistency.

**Resource Safety**: Smart pointers and RAII ensure cleanup on failure, preventing resource leaks.

### Validation Techniques

**Validation Chaining**: Sequential validation from frontend to backend to state checking creates a robust error-catching pipeline.

**Capacity Pre-Checking**: Compatibility validation occurs before assignment attempts to reduce failed operations.

**Timestamped Logging**: System actions recorded with timestamps in `SystemLogWidget` for auditing and debugging.

## Performance Characteristics

### Computational Efficiency

| Operation | Approach | Benefit |
|-----------|----------|---------|
| Vehicle Operations | Direct index access | Lookup independent of fleet size |
| Package Assignment | Pre-calculation with validation | Reduces failed assignment attempts |
| System Statistics | Calculated on demand | Avoids unnecessary data processing |
| UI Updates | Selective refresh | Updates only changed elements |

### Memory Management

**Object Structure**: Vehicle and package objects contain essential data members to minimize memory overhead.

**String Handling**: Uses ***const*** reference returns in getter methods to prevent unnecessary string copies.

**Move Semantics**: Uses move operations and reference passing to transfer ownership ***without copying data***, particularly in package assignments and dialog returns.

**Resource Lifecycle**: Clear ownership hierarchies enable predictable resource management and timely cleanup.

**Widget Hierarchy**: Uses Qt's parent-child relationships for automatic widget cleanup.

## C++ Implementation Patterns

**Const-Correctness**: Uses const qualification for interface contracts, indicating which methods don't modify object state and enabling compiler optimizations.

**Resource Management**: Implements ***move semantics*** and exclusive ownership to eliminate manual pointer management while maintaining performance.

**Polymorphism**: Uses ***virtual methods for runtime behavior specialization**** where different types require different implementations.

**Enum Mapping**: Maps enumeration values to visual colors in UI to provide immediate type and state recognition.

## Build & Deployment

### Development
- **CMake Build System**: Target definitions with dependency management for cross-platform compilation
- **Qt Integration**: Platform abstraction for consistent behavior across operating systems (Cross-Platform)
- **C++17 Standard**: Uses move semantics and smart pointers for modern resource management

### Deployment
- Single executable with Qt runtime dependencies
- In-memory operation requiring no external configuration
- Resource cleanup through RAII principles for automatic management

## Extensibility

The architecture supports extension through:
- [x] **Polymorphic Inheritance**: New types via derivation where behavior differences require specialization
- [x] **Component Architecture**: Additional UI components where new functionality needs separate presentation
- [x] **Factory Methods**: Object creation through component extension where new types need dedicated construction logic
- [x] **Modular Design**: Independent component modifications where changes should be isolated
