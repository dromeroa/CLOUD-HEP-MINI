




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

Se observa una gran densidad de puntos concentrados alrededor de (d_{xy} = 0).

* **Física**: Estas partículas se originan directamente en el **Vértice Primario (PV)**.
* **Contexto de jets**:
  La mayoría de las partículas en jets de quarks ligeros ((u,d,s)) o gluones provienen de la fragmentación inmediata en el punto de colisión.
  Por definición, su distancia mínima al vértice debe ser cero, salvo por la resolución finita del detector.

---

## 3. Dispersión “real” (vida media y *B-tagging*)

Además de la banda central, aparece una “nube” de puntos con (d_{xy} \neq 0), especialmente visible en el rango de (p_T) medio (≈ 1–100 GeV).

* **Física**:
  Parte de esta dispersión no es instrumental, sino **física real**, asociada a la desintegración de hadrones de vida media larga (principalmente hadrones **B** y **C**).
* **Importancia**:
  Un hadrón B puede recorrer varios milímetros antes de decaer. Sus productos de desintegración presentan valores de (d_{xy}) significativamente distintos de cero.
  Este es el principio fundamental de los algoritmos de *B-tagging* (por ejemplo **DeepJet** o **ParticleNet**), que buscan trazas con valores altos de (d_{xy}) o de su significancia (d_{xy}/\sigma).

---

## 4. *Outliers* a bajo (p_T) (artefactos y fondo)

En el extremo izquierdo ((p_T < 1) GeV) se observan puntos con valores extremos de (d_{xy}) (cercanos a (\pm 100)).

### Interpretación

* **Fakes**:
  Trazas mal reconstruidas por el algoritmo de *tracking* debido al alto ruido combinatorio a bajo momento.
* **Interacciones nucleares**:
  Partículas secundarias producidas cuando una partícula del jet interactúa con el material del detector lejos del centro.
* **Pileup**:
  Contaminación de colisiones protón–protón simultáneas, aunque los cortes de calidad suelen mitigar este efecto.

> **Nota técnica**:
> En análisis de física (por ejemplo, para el diseño de un *autoencoder*), es común aplicar cortes como (p_T > 1) GeV para eliminar este ruido, que no aporta información física relevante sobre la naturaleza del jet.

---

 **identificación de *fat jets* (H/Z/W)**:

* Este gráfico muestra que (d_{xy}) es una **variable altamente discriminante**, pero con una fuerte dependencia en (p_T).
* Si estos datos se usan en una red neuronal (GNN o *Autoencoder*), el modelo debe aprender que:

  * un (d_{xy}) grande a bajo (p_T) suele ser ruido,
  * mientras que un (d_{xy}) grande a alto (p_T) es altamente significativo y puede indicar sabor pesado o estructura interna del jet.

---

