# Stochastische Unsicherheitsanalyse

Für die Unsicherheitsberechnung wird weiterhin lineares Materialverhalten (Hooksches Gesetz) angenommen mit :

$$
\underline{K} \cdot \vec{u} = \vec{f} \mid 
\underline{K} = \int_{\Omega} \underline{\mathbb{B}}^T  \underline{\mathbb{E}}   \underline{\mathbb{B}} d\Omega
$$

$\underline{\mathbb{E}}$ ist dabei eine Funktion mehrerer Materialparameter.

$$
\underline{\mathbb{E}} = \underline{\mathbb{E}}(E, \nu)
$$

Dabei ist $E$ das Elastizitäts Modul und gibt die Materialsteifigkeit bei linear elastischer Verformung an und $\nu$ die Poissonzahl auch Querkontraktionszahl, die angibt mit welchem Faktor elastische Verformungen in eine Richtung die Verformung in die anderen Richtungen beeinflusst.

Da sich diese Materialparameter für Bauteile einer Charge aufgrund von Materialunreinheiten/ -fehlern, etc. leicht unterscheiden können muss diese Abweichung bei Betrachtung und Auslegung der gesamten Charge berücksichtigt werden.

Dazu soll im Rahmen dieser Studienarbeit eine Unsicherheitsanalyse implementiert werden, die es ermöglicht Spannungen/Dehnungen und Verformungen eines Systems für eine stochastische Verteilung der Materialparameter zu bestimmen. 
Im Folgenden wird eine stochastische Verteilung $\xi$ angenommen.

Eine mögliche Verteilungsfunktion ist die Normal Verteilung.

$$
p(\xi) = \frac{1}{\sqrt{2\pi}\sigma}\exp\left(-\frac{(\xi-\mu)^2}{2\sigma^2}\right)
$$

Dabei sind $\mu$ und $\sigma$ Erwartungswert und Standardverteilung. Die Funktion gibt die Wahrscheinlichkeitsdichte an der Stelle $\xi$ an.
Die Wahrscheinlichkeit dafür, dass ein Wert zwischen den Stellen $a$ und $b$ liegt ist damit

$$
P(a \leq \xi \leq b) = \int_a^b p(\xi)d\xi
$$

Im folgenden wird der Elastizitätstensor $\underline{\mathbb{E}}$ als stochastisch verteilt angenommen. Diese Verteilung wird mit der Verteilungsfunktion $p = p(\xi)$ beschrieben. Dabei ist $\xi$ die Zufallsvariable mit dem Erwartungswert $0$. Die verteilte Größe $\underline{\mathbb{E}}$ kann damit beschrieben werden als $\underline{\mathbb{E}}(\xi) = \underline{\mathbb{E}}_0 \cdot (1+\xi)$ mit $\underline{\mathbb{E}}_0 = \underline{\mathbb{E}}(E,\nu)$

Damit ergibt sich die Steifigkeitsmatrix des Systems als

$$
\underline{K} = \underline{K}(\xi) = \int_{\Omega} \underline{\mathbb{B}}^T  \underline{\mathbb{E}}_0 \cdot (1+\xi)   \underline{\mathbb{B}}^T d\Omega
$$

Der Fakor $(1+\xi)$ kann aus dem Integral herausgezogen werden.

$$
\underline{K}(\xi) = (1+\xi) \cdot \int_{\Omega} \underline{\mathbb{B}}^T  \underline{\mathbb{E}}_0   \underline{\mathbb{B}}^T d\Omega = (1+\xi) \cdot \underline{K}_0
$$

Setzt man die Steifigkeitsmatrix nun in das globale Gleichungssystem des Systems ein ergibt sich

$$
(1+\xi) \cdot \underline{K}_0 \cdot \vec{u} = \vec{f} \mid \vec{u} = \vec{u}(\xi) 
$$

Dabei ist $\vec{f}$ der feststehende Kraftvektor und $\vec{u}$ der gesuchte Verschiebungsvektor der das durch die Größe $\xi$ variierte System löst. $\vec{u}$ wird im folgenden über die Taylorreihe genähert.

$$
\vec{u}(\xi) = \sum_{n=0}^{\infty} \frac{\vec{u}^{(n)}(\xi_0)}{n!} (\xi - \xi_0)^n
$$

Im Folgenden wird der Verlauf des Verschiebungsvektors $\vec{u}$ über der Zufallsvariablen $\xi$ als linear angenommen und kann somit über eine Taylorreihe mit $2$ Reihengliedern beschrieben werden. Als Stützstelle wird wird $\xi_0 = 0$ gewählt.
Somit vereinfacht sich der gewählte Ansatz zu

$$
\vec{u}(\xi) = \vec{u}^{(0)}(\xi_0) + \xi \cdot \vec{u}^{(1)}(\xi_0) 
$$

Damit ergibt sich für das Gleichungssystem

$$
(1+\xi) \cdot \underline{K}_0 \cdot (\vec{u}^{(0)} + \xi \cdot \vec{u}^{(1)}) = \vec{f}
$$

Nach dem Ausmultiplizieren der linken Seite ergibt sich

$$
\underline{K}_0 \vec{u}^{(0)} + \xi(\underline{K}_0 \vec{u}^{(0)} + \underline{K}_0 \vec{u}^{(1)}) + \xi^2 \underline{K}_0 \vec{u}^{(1)} = \vec{f}
$$

Ableitung $\frac{\partial}{\partial \xi}$ bilden

$$
\underline{K}_0 \vec{u}^{(0)} + \underline{K}_0 \vec{u}^{(1)} = 0
$$

es folgt

$$
\vec{u}^{(1)} = - \vec{u}^{(0)}
$$

damit kann $\vec{u}(\xi)$ bestimmt werden als

$$
\vec{u}(\xi) =  \vec{u}^{(0)} \cdot (1-\xi)
$$

Damit kann die Verschiebung an der Stelle $\underline{\mathbb{E}}_0$ einmalig bestimmt und für alle weiteren $\xi$ über den oben genannten Ansatz bestimmt werden.

$$
\vec{\epsilon}(\xi) = \int_{} \underline{\mathbb{B}} \cdot \vec{u} (\xi)
$$

$$
\vec{\sigma}(\xi) = \int_{} \underline{\mathbb{E}}(\xi) \cdot \underline{\mathbb{B}} \cdot \vec{u} (\xi)
$$

Einsetzen

$$
\vec{\epsilon}(\xi) = (1-\xi) \int_{} \underline{\mathbb{B}} \cdot \vec{u}_0 = (1-\xi) \cdot \vec{\epsilon}_0
$$

$$
\vec{\sigma}(\xi) = (1-\xi^2)\int_{} \underline{\mathbb{E}}_0 \cdot \underline{\mathbb{B}} \cdot \vec{u}_0 = (1-\xi^2) \cdot \vec{\sigma}_0
$$

Damit können auch $\vec{\epsilon}(\xi)$ und $\vec{\sigma}(\xi)$ anhand der Lösung des Gleichungssystems für den Erwartungswert ermittelt werden.