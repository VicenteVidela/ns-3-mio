# Internet QoS Resilience
Este repositorio es un fork del simulador ns-3, donde se implementaron métodos de medición de métricas QoS como ancho de banda, latencia, pérdida de paquetes, etc., y métodos para simular desconexiones abruptas de nodos físicos.
## 1. Prerrequisitos
Este proyecto tiene los mismos requisitos que ns-3 para ser utilizado, los cuales son:
- Sistema Unix (Linux, macOS)
- Compilador C++: `clang++` o `g++` versión 9 o superior
- `Python3.6` o superior
- `CMake` versión 3.10 o superior
- Sistema de compilación: `make, ninja, xcodebuild`

## 2. Compilación
Dado que este fork utiliza ns-3 como base, necesita los mismos pasos para compilar el código. Existen varias opciones, pero aquí se detallará la más directa, adecuada para este repositorio, que tampoco requiere configuraciones adicionales. Los pasos son los siguientes:

1. Para configurar ns-3 con todas sus dependencias, en la carpeta base del proyecto se debe correr el comando `./ns3 configure`
2. Una vez configurado, se debe compilar el código utilizando el comando `./ns3 build`

Una vez listo, el sistema estará compilado con la configuración básica de ns-3, suficiente para este proyecto.

## 3. Uso
### Correr simulaciones
Una vez compilado el proyecto, se puede usar el comando `./ns3 run 'target'` para ejecutar la simulación 'target'. En el caso específico de este proyecto,
existe el programa `5Desconexiones` dentro de la carpeta `scratch`, el cual contiene toda la lógica principal de la versión final de las simulaciones.
El programa por si solo corre una simulación por defecto, pero se le pueden entregar los siguientes parámetros que alteran su funcionamiento, de donde se extraen los datos y donde se guardan:
- `specificTopologyDirectory`: Cuál es el directorio dentro de la carpeta `scratch/topologies` donde se encuentran los archivos de topología, de nodos proveedores y los archivos de los ataques a hacer a la red.
  - El archivo de nodos proveedores es un archivo llamado `proveedores.txt`, el cual especifica los nodos lógicos proveedores de la red
  - Los archivos de ataques son archivos de texto con las extensión `.disc`, que es un arreglo de arreglos, donde cada subarreglo corresponde a una cantidad igual o mayor a 1 de nodos lógicos a desconectar en ese paso.
- `topology`: Archivo csv dentro de `specificTopologyDirectory` que contiene la topología de la red lógica a utilizar en las simulaciones
- `disconnectionsFile`: Archivo `.disc` de donde se extrae el orden de desconexiones de nodos de la simulación a correr.
- `output`: Archivo donde guardar los resultados de la simulación.
- `seed`: Semilla aleatoria, por si se quiere tener control sobre la aleatoriedad de los experimentos.
- `stopTime`: Por cuántos segundos correr la simulación
- `iteration`: Número de iteración de la simulación, para saber cuántos nodos desconectar tomando como referencia el archivo de desconexiones (e.g. en la iteración 5 se desconectarán los nodos especificados desde el primer subarreglo hasta el quinto)

Además, si se quiere correr todos los experimentos, se puede ejecutar el archivo `run.sh`, el cual toma cada uno de los archivos `.disc` de la carpeta (y subcarpetas de) `scratch/topologies/Topologia1` y corre cada iteración durante 50 segundos, luego guardando los resultados en la carpeta `resultados`.

### Visualización resultados
Una vez obtenidos los resultados, estos pueden ser visualizados utilizando los archivos Python de la carpeta `data`:
- `plot.py`: Permite graficar los resultados dado parámetros específicos de las simulaciones. Su uso es `plot.py <topologia> <forma> <imax> <n>`, donde:
  - `topologia`: modelo físico utilizado para generar la topología. Uno de 5NN, ER , GG, GPA, RNG, YAO.
  - `forma`: forma del espacio donde se disponen los nodos, puede ser long o square
  - `imax`: cantidad máxima de interenlaces entre nodos físicos y lógicos de la red interdependiente. Acepta valores igual a 3, 5, 7 y 10.
  - `n`: corresponde al experimento número `n`, el cual puede ser desde 1 a 50. Varía la aleatoriedad y orden en que se eliminan los nodos.
- `aggregateData.py`: Permiter agregar los 50 experimentos asociados un solo set de parámetros, calculando promedio y desviación estándar para cada métrica medida. Su uso es `aggregateData.py <topologia> <forma> <imax>`, donde los parámetros pueden tomar los mismos valores que el anterior. En caso de no entregar parámetros, se calculará la agregación de métricas para cada una de las combinaciones posibles. Los resultados se guardan dentro de la carpeta `AggregatedData/` en un csv con extensión `.dat`
- `aggregateGL.py`: Agrupa los valores de G_L según los mismos parámetros de `aggregateData.py`. En caso de no entregar parámetros, se calculará la agregación de GL para cada una de las combinaciones posibles. Los resultados se guardan dentro de la carpeta `AggregatedData/` en un csv con extensión `.fraction`
- `plotAggregate.py`: Permite graficar los datos ya agrupados según los archivos `aggregateData.py` y `aggregateGL.py`. Al igual que estos, toma los mismos parámetros si se quiere graficar alguna combinación de parámetros en específico, o grafica todos en caso de ejecutarlo sin parámetros. Además, dentro del archivo tiene dos modos, uno para mostrar y otro para guardar los gráficos en archivos `.png`. Hace 5 gráficos, separados según las métricas medidas.
- `correlations.py`: Tomando los datos agregados según los archivos `aggregateData.py` y `aggregateGL.py`, calcula las correlaciones de Pearson, Spearman y Kendall entre cada métrica y G_L. Luego, las guarda en un archivo _json_ dentro de la carpeta `Correlations`. Se puede ejecutar con los mismos parámetros anteriores para el cálculo de un caso, o no entregar parámetros y calcular para todas las combinaciones posibles.
- `plotCorrelations.py`: Genera heatmaps de los valores de correlación de Pearson y Spearman, calculados con el archivo anterior. Para estos calculos, se pueden agrupar los datos según los datos que se desee, entregando como parámetro un número entre 0 y 2:
  - `0`: Agrupar según topología.
  - `1`: Agrupar según forma espacial.
  - `2`: Agrupar según cantidad máxima de interenlaces.

