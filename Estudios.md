
<img width="3000" height="2100" alt="image" src="https://github.com/user-attachments/assets/7d0de187-214c-42cd-9193-0e8ffceb6d7b" />




<img width="3000" height="2400" alt="image" src="https://github.com/user-attachments/assets/2cc4f60a-6d79-4b3e-af44-1adddfc035e7" />


# Distribución espacial de constituyentes del jet en el plano $\eta-\phi$

Este gráfico visualiza la **distribución espacial de los constituyentes del jet** en el plano $\eta-\phi$, utilizando el color para representar el valor del **parámetro de impacto transversal** $d_{xy}$.

Esta visualización es crucial para entender la **calidad** y la **geometría** de los datos antes de utilizarlos como entrada en un modelo de *Machine Learning*. A continuación se presenta una interpretación detallada, orientada a tu investigación en **CMS**.

---

## 1. Geometría del detector y aceptancia del *tracker*

### Rango en pseudorrapidez $|\eta| < 2.5$

Se observa que los puntos se cortan abruptamente alrededor de $\eta \approx \pm 2.5$.

* **Física**:
  Este límite corresponde a la cobertura física del **Silicon Tracker** de CMS.
  Aunque los calorímetros (ECAL/HCAL) se extienden hasta $|\eta| = 3.0$ o incluso (5.0), el cálculo de $d_{xy}$ requiere la reconstrucción de **trazas** (*tracks*).

* **Implicancia**:
  Para un identificador de *fat jets*, cualquier información que dependa de $d_{xy}$ (por ejemplo *b-tagging* o variables de vida media) solo estará disponible en esta región central.
  Si el *dataset* incluye jets con $|\eta| > 2.5$, esta variable será nula o inválida.

---

## 2. Uniformidad y *pileup*

La distribución cubre todo el rango de $\phi -\pi$ a $+\pi$ y el rango central de $\eta$ de manera casi uniforme, con un patrón tipo “sal y pimienta”.

* **Interpretación**:
  Esto indica que se está visualizando una muestra acumulada de muchos eventos (o eventos con alto *pileup*).
  En colisionadores, la producción de partículas es aproximadamente isotrópica en el ángulo azimutal $\phi$.

* **Chequeo de calidad**:
  La ausencia de “agujeros” grandes sugiere que no existen sectores muertos significativos del detector en este conjunto de datos.

---

## 3. Mapa de calor de $d_{xy}$ (mezcla rojo/azul)

Los colores representan el valor de $d_{xy}$ en una escala de aproximadamente $\pm 0.1\ \text{cm}$.

* **Aleatoriedad**:
  Se observa una mezcla homogénea de puntos rojos ($d_{xy} > 0$) y azules ($d_{xy} < 0$), sin regiones dominadas por un solo signo.

* **Validación de alineación**:
  Esta es una buena señal. El signo de $d_{xy}$ depende de la geometría de la traza con respecto al *beam spot*.
  Una mala alineación del detector o un cálculo incorrecto del *beam spot* produciría una dependencia sinusoidal en $\phi$ (por ejemplo, todo rojo en ($\phi = 0$) y todo azul en ($\phi = \pi$).
  La mezcla aleatoria indica que los datos están globalmente bien calibrados.

---

## 4. Estructura fina (las “rayas” verticales)

Se aprecian variaciones sutiles en la densidad de puntos a lo largo del eje ($\eta$), visibles como “rayas” verticales.

* **Física**:
  Estas estructuras suelen reflejar transiciones mecánicas del detector:

  * La transición entre el **barrel** (región central) y los **endcaps** del *tracker* ocurre alrededor de ($|\eta| \approx 1.4$).
  * En estas zonas hay mayor cantidad de material (cables, soportes), lo que puede afectar la eficiencia de reconstrucción y aumentar la dispersión múltiple, degradando la precisión de $d_{xy}$.

---

## Conexión con tu *Autoencoder* y *fat jets*

Para tu proyecto de **identificación de señales (H/Z/W)**:

* **Preprocesamiento**:
  Al entrenar la red (ParticleNet, CNN o *Autoencoder*), es importante evitar que el modelo “aprenda” la geometría del detector en lugar de la física del jet.
  Las ineficiencias locales en (\eta) no deberían ser usadas como criterio de clasificación.

* **Normalización**:
  Aunque el rango de colores aquí es ($\pm 0.1$), en distribuciones globales pueden aparecer valores de ($|d_{xy}|$) de hasta ($\mathcal{O}(100)$) debido al ruido.
  Es fundamental **recortar (*clip*) los valores extremos** antes de normalizar:

  * Rangos típicos recomendados: ([-0.1, 0.1]) o ([-0.2, 0.2]).
  * Esto captura la física relevante de hadrones B sin introducir colas dominadas por ruido.

---





<img width="3000" height="2100" alt="image" src="https://github.com/user-attachments/assets/72394417-c8c7-49e4-89a6-26482c567c80" />





# Relación entre ($p_T$) y ($d_{xy}$) en constituyentes de jets (CMS)

Este gráfico muestra la relación entre el **momento transversal** ($p_T$) y el **parámetro de impacto transversal** ($d_{xy}$) para los constituyentes de un jet (candidatos del algoritmo *Particle Flow*) en el experimento **CMS**.

A continuación se presenta una **interpretación física detallada** de las características observadas.

---

## 1. Forma de “Embudo” (Resolución del detector)

La característica más dominante es que la dispersión de los valores de ($d_{xy}$) es muy amplia a bajo ($p_T$) y se estrecha significativamente a medida que aumenta el ($p_T$).

* **Física**: Consecuencia directa de la resolución de las trazas en el detector interno (*tracker*).
* **Explicación**:
  Las partículas de bajo momento sufren mayor **Dispersión Múltiple de Coulomb** (*Multiple Coulomb Scattering*) al atravesar las capas de silicio del detector.
  Esto degrada la reconstrucción de su trayectoria, aumentando la incertidumbre en su punto de origen ($d_{xy}$).
  Las partículas de alto ($p_T$) son más “rígidas”, se curvan y dispersan menos, permitiendo una extrapolación mucho más precisa hacia el vértice primario.

---

## 2. Banda central (Partículas *prompt*)

Se observa una gran densidad de puntos concentrados alrededor de ($d_{xy}$ = 0).

* **Física**: Estas partículas se originan directamente en el **Vértice Primario (PV)**.
* **Contexto de jets**:
  La mayoría de las partículas en jets de quarks ligeros ((u,d,s)) o gluones provienen de la fragmentación inmediata en el punto de colisión.
  Por definición, su distancia mínima al vértice debe ser cero, salvo por la resolución finita del detector.

---

## 3. Dispersión “real” (vida media y *B-tagging*)

Además de la banda central, aparece una “nube” de puntos con ($d_{xy} \neq 0$), especialmente visible en el rango de $p_T$ medio (≈ 1–100 GeV).

* **Física**:
  Parte de esta dispersión no es instrumental, sino **física real**, asociada a la desintegración de hadrones de vida media larga (principalmente hadrones **B** y **C**).
* **Importancia**:
  Un hadrón B puede recorrer varios milímetros antes de decaer. Sus productos de desintegración presentan valores de $d_{xy}$ significativamente distintos de cero.
  Este es el principio fundamental de los algoritmos de *B-tagging* (por ejemplo **DeepJet** o **ParticleNet**), que buscan trazas con valores altos de $d_{xy}$ o de su significancia ($d_{xy}/\sigma$).

---

## 4. *Outliers* a bajo ($p_T$) (artefactos y fondo)

En el extremo izquierdo ($p_T$ < 1 GeV) se observan puntos con valores extremos de $d_{xy}$ (cercanos a ($\pm 100$)).

### Interpretación

* **Fakes**:
  Trazas mal reconstruidas por el algoritmo de *tracking* debido al alto ruido combinatorio a bajo momento.
* **Interacciones nucleares**:
  Partículas secundarias producidas cuando una partícula del jet interactúa con el material del detector lejos del centro.
* **Pileup**:
  Contaminación de colisiones protón–protón simultáneas, aunque los cortes de calidad suelen mitigar este efecto.

> **Nota técnica**:
> En análisis de física (por ejemplo, para el diseño de un *autoencoder*), es común aplicar cortes como ($p_T$ > 1) GeV para eliminar este ruido, que no aporta información física relevante sobre la naturaleza del jet.

---

 **identificación de *fat jets* (H/Z/W)**:

* Este gráfico muestra que $d_{xy}$ es una **variable altamente discriminante**, pero con una fuerte dependencia en $p_T$.
* Si estos datos se usan en una red neuronal (GNN o *Autoencoder*), el modelo debe aprender que:

  * un $d_{xy}$ grande a bajo $p_T$ suele ser ruido,
  * mientras que un $d_{xy}$ grande a alto $p_T$ es altamente significativo y puede indicar sabor pesado o estructura interna del jet.

---

