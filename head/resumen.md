# Resumen de Cambios del Commit

## Alcance
Se implementaron mejoras en la estructura Heap del proyecto para volverla mas robusta, concurrente y extensible, ademas de incorporar demo y soporte de lectura/escritura por streams.

## Cambios principales en Heap

1. Nodo base reutilizable
- Se adopto `BaseNode<T>` como base comun para `HeapNode<T>`.
- Se mantiene consistencia con los nodos de listas y vector.

2. Heap concurrente
- Se protege el estado interno con `std::mutex` (`m_mtx`).
- Operaciones publicas bloqueadas con `std::scoped_lock`.
- Se agrego destructor seguro (`~Heap() noexcept`) con limpieza protegida.

3. API funcional del Heap
- Insercion y extraccion:
  - `insert(...)`
  - `extract()`
  - `extract_top()`
  - `try_extract_top(...)`
- Consulta:
  - `peek()`
  - `peek_node()`
  - `peek_ref()`
  - `try_peek_node(...)`
- Estado:
  - `empty()`
  - `size()`
  - `clear()`

4. Construccion y validacion
- Se agrego construccion masiva:
  - `build_from_vector(const std::vector<Node>&)`
  - `build_from_vector(const std::vector<std::pair<value_type, Ref>>& )`
- Se agrego `validate_heap()` para verificar la propiedad del heap.
- Se implemento `heapify_all()` para reconstruccion global O(n).

5. Cambio de comparador y recolocacion
- `change_comp(const Comp&)` permite actualizar comparador del mismo tipo y reordenar.
- `change_comp_to<NewTraits>()` permite convertir a otro heap con otro trait/comparador y recolocar datos.

6. Streams y serializacion
- Se mantiene `operator<<` para imprimir heap.
- Se mantiene `operator>>` para cargar heap desde texto usando `fromIstream(...)`.
- `fromIstream` ahora usa `build_from_vector(...)` para reconstruccion eficiente.

## Demo actualizado
En `HeapDemo.cpp` se agregaron ejemplos de:
- Carga desde archivo con `operator>>`.
- Impresion con `operator<<`.
- `validate_heap()`.
- `build_from_vector(...)`.
- Conversion de comparador con `change_comp_to<...>()`.

## Resultado
Se obtuvo un Heap mas completo para uso academico:
- Concurrente
- Mas robusto ante casos vacios
- Con operaciones de construccion/validacion
- Con soporte de conversion de comparador y demo funcional
