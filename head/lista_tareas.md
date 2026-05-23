# Lista de Tareas del Commit

## Estado general
Todas las tareas listadas en este archivo estan completadas.

## Tareas completadas

- [x] Crear `BaseNode<T>` como base comun de nodos.
- [x] Migrar `HeapNode<T>` para heredar de `BaseNode<T>`.
- [x] Mantener compatibilidad de traits para heap (`BaseHeapTrait`, `AscendingHeapTrait`, `DescendingHeapTrait`).
- [x] Implementar heap concurrente con `std::mutex` y `std::scoped_lock`.
- [x] Agregar destructor seguro `~Heap() noexcept`.
- [x] Implementar `insert(...)` con reordenamiento por `heapify_up`.
- [x] Implementar `extract()` y `extract_top()`.
- [x] Implementar versiones no-excepcion:
  - [x] `try_peek_node(...)`
  - [x] `try_extract_top(...)`
- [x] Implementar operaciones de consulta:
  - [x] `peek()`
  - [x] `peek_node()`
  - [x] `peek_ref()`
- [x] Implementar operaciones de estado:
  - [x] `empty()`
  - [x] `size()`
  - [x] `clear()`
- [x] Implementar `build_from_vector(...)` (2 sobrecargas).
- [x] Implementar `validate_heap()`.
- [x] Implementar cambio de comparador y recolocacion:
  - [x] `change_comp(...)`
  - [x] `change_comp_to<NewTraits>()`
- [x] Optimizar reconstruccion total mediante `heapify_all()`.
- [x] Mantener serializacion por streams:
  - [x] `operator<<` del heap
  - [x] `operator>>` del heap
  - [x] `fromIstream(...)` con reconstruccion por `build_from_vector(...)`
- [x] Actualizar `HeapDemo.cpp` con ejemplos de:
  - [x] carga por archivo
  - [x] validacion
  - [x] build masivo
  - [x] cambio de comparador
- [x] Actualizar diagrama de herencia para reflejar estado actual de nodos y heap.
