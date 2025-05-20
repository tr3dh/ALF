# Newton Raphson Verfahren

Mit dem Newton Raphson Verfahren lassen sich die Nullstellen nicht linearer Residuuen $\underline{R}$ bestimmen.
Das Newton Raphson Verfahren ist iterativ und nähert seinen 
Arbeitspunkt für den $n$-ten Zeintschritt $\underline{P} = [\vec{\mathbb{x_{n}}}, \underline{R}(\vec{\mathbb{x_{n}}})]$ an die Nullstelle des Residuums an.
Da der Funktionswert des Residuums für die gesuchte Stelle $\underline{R} = 0$ bekannt ist bezeichnen wir fortan die Stelle $\mathbb{x_{n}}$ als Arbeitspunkt.

Die Annäherung des Arbeitspunktes an die Nullstelle geschieht über die die wiederholte Lösung der linearisierten Gleichung um den Arbeitspunkt.
Dabei wird in jedem Iterationschritt die Tangente ans Residuum am Arbeitspunkt angelegt und die Nullstelle dieser gefunden. Die Nullstelle dient als Arbeitspunkt für den nächsten Iterationsschritt.

Die Tangente an das Residuum am Arbeitspunkt:

$$
T(\vec{\mathbb{x_{n}}}) = m*\vec{\mathbb{x_{n}}}+b
$$

$m$ ergibt sich aus der Steigung des Residuums an der Stelle $\vec{\mathbb{x_{n}}}$ und b lässt sich über Einsetzen in die Tangentengleichung ermitteln. Der Funktionswert der Tangente am Arbeitspunkt muss der des Residuums am Arbeitspunkt entsprechen

$$
m = \dot{\mathbb{R}}(x_{n})
$$

$$
\mathbb{R}(\vec{x_n}) = T(\vec{\mathbb{x_n}}) = m*\vec{\mathbb{x}}+b
$$

Damit ergibt sich $b$ zu
$$
b = \mathbb{R}(x_{n})-{m*\vec{\mathbb{x_{n}}}} = \mathbb{R}(x_{n})-{\dot{\mathbb{R}}(x_{n})*\vec{\mathbb{x_{n}}}}
$$

Die Tangente ergibt sich also zu

$$
T(\vec{\mathbb{x_{n}}}) = \dot{\mathbb{R}}(x_{n}) * \vec{x} + \mathbb{R}(x_{n})-{\dot{\mathbb{R}}(x_{n})*\vec{\mathbb{x_{n}}}}
$$

und die Nullstelle der Tangente ist

$$
\vec{x_{n+1}} = x | T(\vec{\mathbb{x_{n}}}) = 0 
$$

$$
\vec{x_{n+1}} = \frac{-\mathbb{R}(x_{n})+{\dot{\mathbb{R}}(x_{n})*\vec{\mathbb{x_{n}}}}}{\dot{\mathbb{R}}(x_{n})}
$$

Vereinfachen
$$
\vec{x_{n+1}} = -\frac{\mathbb{R}(x_{n})}{\dot{\mathbb{R}}(x_{n})}+\vec{\mathbb{x_{n}}}
$$

anders schreiben
$$
\vec{x_{n+1}} = \vec{\mathbb{x_{n}}} +\Delta \vec{\mathbb{x_{n}}} | \Delta \vec{\mathbb{x_{n}}} = -\frac{\mathbb{R}(x_{n})}{\dot{\mathbb{R}}(x_{n})}
$$

Die Iteration wird abgebrochen sobald das Residuum Null ist, also die Nullstelle exakt bestimmt wurde oder sich der Restbetrag innerhalb einer gewissen Toleranz um den Funktionswert Null herum befindet als:

$$
while(\left| \mathbb{R}(\vec{x_{n}}) \right| > tolerance)\{ 
Iterationsschritt
\}\\
$$

# Newton Rahpson in der Fem
Innerhalb der Fem Berechnungen wird der Newton Raphson Solver dafür eingesetzt das Gleihungssystem $\underline{K} \cdot \vec{u} = \vec{f}$ zu lösen. Dabei wird das Gleichungssystem in die Form $\underline{R}(\vec{u}) = \underline{K} \cdot \vec{u}$ gebracht und mit Newton Raphson die Nullstellen Suche durchgeführt $\underline{R}(\vec{u}) \overset{!}{=} \vec{0}$