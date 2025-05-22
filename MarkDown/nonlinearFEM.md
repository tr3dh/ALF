# nichtlineare Materialmodelle

In der nichtlinearen Fem wird nicht mehr rein elastisches Materialverhalten vorrausgesetzt. Um bleibende, plastische Verformungen, Schädigungen und viskoses Fließen innerhalb des Materials zu simulieren wird der lineare Ansatz für die Spannungs/Dehnungsbeziehung, die für den linearen Fall übers Hooksche Gesetz gegeben ist mit $\sigma = \underline{\mathbb{E}}\cdot\varepsilon$ erweitert.
Für viskoses Materialverhalten wird der Ansatz $\sigma = \underline{\mathbb{E}}\cdot(\varepsilon-\varepsilon^v)$ gewält. Die Spannung wird um einen Wert ${\mathbb{E}}\cdot\varepsilon^v$ reduziert der den Spannungswegfall aufgrund von Nachfließen des Materials in den verformten Zustand reduziert. Die Änderung der viskosen Dehnung $\dot{\varepsilon}^v$ ist dabei proportinal zum anliegenden Spannungszustand $\dot{\varepsilon}^v \propto \sigma$. Ein guter Vergleich hierfür ist das viskose Verhalten eines Dämpfers bei dem eine Gegenkraft zur Kolbenbewegung entsteht die Proportional zur Bewegungsgeschwindigkeit ist. Die Evolutionsgleichung von der viskosen Dehnung ergibt sich als:

$$
\dot{\varepsilon}^v = \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \sigma \mid \sigma = \underline{\mathbb{E}}\cdot(\varepsilon-\varepsilon^v)
$$

$\eta$ ist dabei ein Materialparamter der die Ausprägung der Viskoelastizität angibt. $\underline{\mathbb{S}}$ ist ein Deviator der den Volumenerhalt sicherstellt. Bei der Multiplikation von Deviator und Spannung ergibt sich der Spannungsdeviator. Die Fortplanzung der viskoelastischen Dehnung in den nächsten Zeitschritt ist über die Evolutiongleichung möglich. Hierbei wird Fortplanzung implizit durchgeführt also $\dot{\varepsilon}^v_{n+1} = f(\varepsilon^v_{n+1}, ..._{n+1})$ und nicht explizit $\dot{\varepsilon}^v_{n+1} = f(\varepsilon^v_{n}, ..._{n})$

Also lässt sich die implizite Fortplanzung von $\dot{\varepsilon}^v$ anstellen als

$$
\dot{\varepsilon}^v_{n+1} = \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot(\varepsilon_{n+1}-\varepsilon^v_{n+1})
$$

und

```math
\varepsilon^v_{n+1} = \varepsilon^v_{n}+\Delta t \cdot \dot{\varepsilon}^v_{n+1}
```

Die Ableitung von ${\varepsilon}^v_{n+1}$ lässt sich als die Sekantensteigung an $\varepsilon^v$ ermitteln über das Anlegen der Sekante an die beiden Punkte ${\varepsilon}^v_{n+1}$

```math
\dot{\varepsilon}^v_{n+1} = \frac{\varepsilon^v_{n+1} - \varepsilon^v_{n}}{\Delta t}
```

Damit ergibt sich ein Zusammenhang der nur noch die gesuchten Größe $\varepsilon^v_{n+1}$ enthält und davon abgesehen bekannte Größen.

```math
\frac{\varepsilon^v_{n+1} - \varepsilon^v_{n}}{\Delta t} = \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot(\varepsilon_{n+1}-\varepsilon^v_{n+1})
```

Damit lässt sich die Evolutionsgleichung nach der einzig unbekannten Größe $\varepsilon^v_{n+1}$ umstellen als

```math
\frac{\varepsilon^v_{n+1} - \varepsilon^v_{n}}{\varepsilon_{n+1} - \varepsilon^v_{n+1}} = \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t
```

Im folgenden wird die rechte Seite als $A$ substituiert

```math
A := \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t
```

Damit ergibt sich die umgeformte Evolutionsgleichung zu

```math
\frac{\varepsilon^v_{n+1} - \varepsilon^v_{n}}{\varepsilon_{n+1} - \varepsilon^v_{n+1}} = A
```
```math
\Leftrightarrow \varepsilon^v_{n+1} - \varepsilon^v_{n} = A \cdot (\varepsilon_{n+1} - \varepsilon^v_{n+1})
```
```math
\Leftrightarrow \varepsilon^v_{n+1} - \varepsilon^v_{n} = A \cdot \varepsilon_{n+1} - A \cdot \varepsilon^v_{n+1}
```
```math
\Leftrightarrow \varepsilon^v_{n+1} + A \cdot \varepsilon^v_{n+1} = A \cdot \varepsilon_{n+1} + \varepsilon^v_{n}
```
```math
\Leftrightarrow (1 + A) \cdot \varepsilon^v_{n+1} = A \cdot \varepsilon_{n+1} + \varepsilon^v_{n}
```

Löse nach $\varepsilon^v_{n+1}$ auf:

```math
\varepsilon^v_{n+1} = \frac{A \cdot \varepsilon_{n+1} + \varepsilon^v_{n}}{1 + A}
```

Setze wieder $A$ ein:

```math
\varepsilon^v_{n+1} = [{1 + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t}]^{-1} \cdot [\varepsilon^v_{n} + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t \cdot \varepsilon_{n+1}]
```

Mit der Operator Matrix

```math
\underline{\mathbb{S}} = 
\begin{bmatrix}
\frac{2}{3} & -\frac{1}{3} & -\frac{1}{3} & 0 & 0 & 0 \\
-\frac{1}{3} & \frac{2}{3} & -\frac{1}{3} & 0 & 0 & 0 \\
-\frac{1}{3} & -\frac{1}{3} & \frac{2}{3} & 0 & 0 & 0 \\
0 & 0 & 0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 & 1 & 0 \\
0 & 0 & 0 & 0 & 0 & 1 \\
\end{bmatrix}
```

Das Residuum für den jeweils nächsten Iterationsschritt $i+1$ ergibt sich dann zu

```math
\underline{R}_{i+1} = \int_{\Omega} \underline{B}^T \cdot \sigma_{i+1} dV
```
```math
\Leftrightarrow \underline{R}_{i+1} = \int_{\Omega} \underline{B}^T \cdot \underline{\mathbb{E}} \cdot(\varepsilon_{i+1}-\varepsilon_{i+1}^v) dV \mid \varepsilon_{i+1} = \underline{B} \cdot \vec{u}_{i+1}
```

Damit ergibt sich die Tangente zu

```math
\underline{T}_{i+1} = \int_{\Omega} \underline{B}^T \cdot \frac {\partial \sigma}{\partial \varepsilon} \cdot \underline{B} dV
```

Wobei $\sigma$ eine Funktion von $\varepsilon$ und $\varepsilon^v$ ist

```math
\sigma(\varepsilon,\varepsilon^v) = \underline{\mathbb{E}}\cdot(\varepsilon-\varepsilon^v) 
```

Dementsprechend ergibt sich der Ableitungsterm $\frac {\partial \sigma}{\partial \varepsilon}$ zu

```math
\frac {\partial \sigma}{\partial \varepsilon} = \frac {\partial \sigma}{\partial \varepsilon} + \frac {\partial \sigma}{\partial \varepsilon^v} \cdot \frac {\partial \varepsilon^v}{\partial \varepsilon}
```

Wobei 
```math
\frac {\partial \sigma}{\partial \varepsilon} = \underline{\mathbb{E}} \quad \frac {\partial \sigma}{\partial \varepsilon^v} = -\underline{\mathbb{E}} \quad
```

Für $\frac {\partial \varepsilon^v}{\partial \varepsilon}$ muss die Evolutionsgleichung abgeleitet werden

```math
\frac{\partial \varepsilon^v_{n+1}}{\partial \varepsilon_{n+1}} = [{1 + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t}]^{-1} \cdot [\varepsilon^v_{n} + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t]
```

Eingesetzt in die Tangente

```math
\underline{T}_{i+1} = \int_{\Omega} \underline{B}^T \cdot 
\underline{\mathbb{E}}-
\underline{\mathbb{E}} \cdot 
[{1 + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t}]^{-1} \cdot [\varepsilon^v_{n} + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t]
\cdot \underline{B} dV
```

Damit kann ein schritt fürs Newton Raphson Verfahren durchgeführt werden.
Da die KMatrix nicht abhängig von u ist wird das Newton Raphson Verfahren in einem Schritt konvergieren.
Dazu wird für die viskoelastische Dehnung zuert der Startwert 0 angenommen und im weiteren Verlauf die berechnenten Größen an den Quadraturpunkten für jeden Berechnungsschritt gespeichert.

Das bedeutet das die Berechnung nach folgendem Schema abläuft
- $\varepsilon_{i=0}^v = 0$
- $\underline{R}_{0} = \int_{\Omega} \underline{B}^T \cdot \underline{\mathbb{E}} \cdot(\varepsilon_{0}-\varepsilon_{0}^v) dV \mid \varepsilon_{0} = \underline{B} \cdot \vec{u}_{0}$
- mit NR lösen und $u_0$ -> $\varepsilon_0$ ermitteln
---
while $\Delta t \cdot i < t_{end}$
- $\varepsilon^v_{n+1} = [{1 + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t}]^{-1} \cdot [\varepsilon^v_{n} + \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \Delta t \cdot \varepsilon_{n+1}]$
- $\varepsilon^v_{n+1}$ fortplanzen, $\varepsilon_{i+1}$ durch $\underline{B} \cdot \vec{u}_{i+1}$ ersetzen und ins Residuum mitreinnehmen
- $\underline{R}_{i+1} = \int_{\Omega} \underline{B}^T \cdot \underline{\mathbb{E}} \cdot(\varepsilon_{i+1}-\varepsilon_{i+1}^v) dV \mid \varepsilon_{i+1} = \underline{B} \cdot \vec{u}_{i+1}$
- NR lösen und $\vec{u}_{i+1}$ -> $\vec{\varepsilon}_{i+1}$ bestimmen
---