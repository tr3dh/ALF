# nichtlineare Materialmodelle

In der nichtlinearen Fem wird nicht mehr rein elastisches Materialverhalten vorrausgesetzt. Um bleibende, plastische Verformungen, Schädigungen und viskoses Fließen innerhalb des Materials zu simulieren wird der lineare Ansatz für die Spannungs/Dehnungsbeziehung, die für den linearen Fall übers Hooksche Gesetz gegeben ist mit $\sigma = \underline{\mathbb{E}}\cdot\epsilon$ erweitert.
Für viskoses Materialverhalten wird der Ansatz $\sigma = \underline{\mathbb{E}}\cdot(\epsilon-\epsilon^v)$ gewält. Die Spannung wird um einen Wert ${\mathbb{E}}\cdot\epsilon^v$ reduziert der den Spannungswegfall aufgrund von Nachfließen des Materials in den verformten Zustand reduziert. Die Änderung der viskosen Dehnung $\dot{\epsilon}^v$ ist dabei proportinal zum anliegenden Spannungszustand $\dot{\epsilon}^v \propto \sigma$. Ein guter Vergleich hierfür ist das viskose Verhalten eines Dämpfers bei dem eine Gegenkraft zur Kolbenbewegung entsteht die Proportional zur Bewegungsgeschwindigkeit ist. Die Evolutionsgleichung von der viskosen Dehnung ergibt sich als:

$$
\dot{\epsilon}^v = \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot \sigma \mid \sigma = \underline{\mathbb{E}}\cdot(\epsilon-\epsilon^v)
$$

$\eta$ ist dabei ein Materialparamter der die Ausprägung der Viskoelastizität angibt. $\underline{\mathbb{S}}$ ist ein Deviator der den Volumenerhalt sicherstellt. Bei der Multiplikation von Deviator und Spannung ergibt sich der Spannungsdeviator. Die Fortplanzung der viskoelastischen Dehnung in den nächsten Zeitschritt ist über die Evolutiongleichung möglich. Hierbei wird Fortplanzung implizit durchgeführt also $\dot{\epsilon}^v_{n+1} = f(\epsilon^v_{n+1}, ..._{n+1})$ und nicht explizit $\dot{\epsilon}^v_{n+1} = f(\epsilon^v_{n}, ..._{n})$

Also lässt sich die implizite Fortplanzung von $\dot{\epsilon}^v$ anstellen als

$$
\dot{\epsilon}^v_{n+1} = \eta^{-1} \cdot \underline{\mathbb{S}} \cdot \underline{\mathbb{E}} \cdot(\epsilon_{n+1}-\epsilon^v_{n+1})
$$