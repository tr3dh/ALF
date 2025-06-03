# Finite-Elemente-Formulierung: Zusammenhang von Steifigkeitsmatrix, Spannung und Residuum

Dieses Dokument erklärt, wie aus der klassischen Finite-Elemente-Gleichung mit der Steifigkeitsmatrix die Residuenformulierung mit Spannungen abgeleitet wird.

---

## 1. Grundformulierung

Die klassische lineare Gleichgewichtsbedingung in der FEM lautet

```math
\underline{K} \cdot \vec{u} = \vec{f}
```

wobei die Steifigkeitsmatrix wie folgt definiert ist:

```math
\underline{K} = \int_{\Omega} \underline{\mathbb{B}}^T \, \underline{\mathbb{E}} \, \underline{\mathbb{B}} \, d\Omega
```

---

## 2. Iterative Lösung bei nichtlinearem Materialverhalten

Für plastische oder nichtlineare Materialien löst man die Gleichung iterativ und über die Nullstellenbestimmung für eine Residuumsfunktion. Das Residuum der $(i+1)$-ten Iteration ist:

```math
\vec{R}_{i+1} := \vec{f} - \underline{K} \cdot \vec{u}_{i+1} \overset{!}{=}  0
```

---

## 3. Umformung in kontinuierliche Form

Wir setzen die Definition von $\underline{K}$ ein:

```math
\underline{K} \cdot \vec{u}_{i+1} = \int_{\Omega} \underline{B}^T \, \underline{\mathbb{E}} \, \underline{B} \cdot \vec{u}_{i+1} \, d\Omega
```

Die Dehnung ergibt sich über den Verschiebungs-Vektor:

```math
\varepsilon_{i+1} = \underline{B} \cdot \vec{u}_{i+1}
```

---

## 4. Spannungsgestützte Formulierung des inneren Kraftvektors

Daraus ergibt sich:

```math
\underline{K} \cdot \vec{u}_{i+1} = \int_{\Omega} \underline{B}^T \cdot \underline{\mathbb{E}} \cdot \varepsilon_{i+1} \, d\Omega
= \int_{\Omega} \underline{B}^T \cdot \sigma_{i+1} d\Omega
```