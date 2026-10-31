<div align="center">

# 🕹️ JoyDuo

### *Un solo joystick, dos servos*

Controla un servo de **270°** y un servo de **rotación continua (360°)** con un único joystick I2C. Pulsa el joystick para cambiar de servo, y mira el estado en tiempo real en una pantalla OLED.

![Plataforma](https://img.shields.io/badge/plataforma-Arduino%20%2F%20KidsUno-00979d)
![Lenguaje](https://img.shields.io/badge/lenguaje-C%2B%2B%20(Arduino)-blue)
![Servos](https://img.shields.io/badge/servos-2-orange)
![Pantalla](https://img.shields.io/badge/OLED-128×64%20SSD1306-lightgrey)
![Comunicación](https://img.shields.io/badge/joystick-I2C%200x39-brightgreen)
![Licencia](https://img.shields.io/badge/licencia-MIT-green)

</div>

---

## 📖 Tabla de contenidos

1. [¿Qué es JoyDuo?](#-qué-es-joyduo)
2. [Características](#-características)
3. [Requisitos](#-requisitos)
4. [Conexiones](#-conexiones)
5. [Instalación](#-instalación)
6. [Cómo usarlo](#-cómo-usarlo)
7. [Modos de control](#-modos-de-control)
8. [Pantalla OLED y LED integrado](#-pantalla-oled-y-led-integrado)
9. [Estructura del código](#-estructura-del-código)
10. [Detalles técnicos](#-detalles-técnicos)
11. [Personalización](#-personalización)
12. [Solución de problemas](#-solución-de-problemas)
13. [Limitaciones conocidas](#-limitaciones-conocidas)
14. [Ideas para el futuro](#-ideas-para-el-futuro)
15. [Contribuir](#-contribuir)
16. [Licencia](#-licencia)

---

## 🎛️ ¿Qué es JoyDuo?

**JoyDuo** es un proyecto de Arduino que permite manejar **dos servos distintos con un solo joystick**. El joystick (módulo **Kidsbits** por I2C) tiene un botón integrado que funciona como selector: cada pulsación cambia el servo que está bajo control.

- **Servo de 270°** (posicional): el joystick lo mueve poco a poco hasta el ángulo deseado, como una pequeña grúa o brazo.
- **Servo de 360°** (rotación continua): el joystick controla el sentido y la velocidad del giro, como las ruedas de un carrito.

Una pantalla **OLED SSD1306** muestra el estado del joystick, el valor del eje Y, el servo seleccionado y el movimiento actual. El LED integrado de la placa indica de un vistazo qué servo está activo.

> Repositorio: <https://github.com/miusarname2/JoyDuo>

---

## ✨ Características

- 🔀 **Dos servos, un joystick:** cambia de servo con solo pulsar el joystick.
- 🎯 **Calibración automática** del centro del eje Y al arrancar (32 muestras).
- 🛡️ **Zona muerta** en el servo de 270° y **histéresis** en el de 360° para evitar movimientos accidentales.
- 🐢🐇 **Velocidad proporcional:** cuanto más empujas el joystick, más rápido se mueve el servo.
- 🔒 **Arranque seguro del servo 360°:** no se mueve hasta que el joystick esté centrado y confirmado.
- 🔌 **Detección de fallos I2C:** si el joystick no responde, el servo 360° se desconecta y el LED parpadea.
- 🖥️ **Pantalla OLED** con información en vivo (refresco cada 250 ms).
- 📟 **Monitor serie** con reportes cada 500 ms para depuración.
- 🧼 **Antirrebote** del botón (60 ms) y rearme tras soltar, para evitar cambios dobles.
- 📦 **Un solo archivo fuente**, fácil de leer y modificar.

---

## 🧰 Requisitos

### Hardware

| Componente | Detalle |
|------------|---------|
| Placa | **KidsUno** o compatible con Arduino UNO |
| Joystick | Módulo **Kidsbits** con interfaz I2C (dirección `0x39`) |
| Servo posicional | Servo de **270°** (pin `D12`) |
| Servo continuo | Servo de **rotación continua / 360°** (pin `D13`) |
| Pantalla | OLED **SSD1306 128×64** por I2C (dirección `0x3C`) |
| Alimentación | Fuente adecuada para los servos (ver nota abajo) |

> ⚡ **Importante:** los servos consumen bastante corriente. Si tu placa se reinicia o la pantalla parpadea cuando se mueven, alimenta los servos con una fuente externa y **une todas las tierras (GND)**.

### Software

- [Arduino IDE](https://www.arduino.cc/en/software) (o [PlatformIO](https://platformio.org/)).
- Librerías:

| Librería | Origen |
|----------|--------|
| `Adafruit GFX Library` | Gestor de librerías de Arduino |
| `Adafruit SSD1306` | Gestor de librerías de Arduino |
| `Servo` | Incluida con Arduino |
| `Wire` | Incluida con Arduino |

---

## 🔌 Conexiones

| Elemento | Pin / Bus | Notas |
|----------|-----------|-------|
| Servo 270° (señal) | `D12` | Alimentación y GND según el servo |
| Servo 360° (señal) | `D13` | Alimentación y GND según el servo |
| Joystick I2C | `SDA` / `SCL` | Dirección `0x39` |
| Pantalla OLED | `SDA` / `SCL` | Dirección `0x3C` |
| LED indicador | `LED_BUILTIN` | Apagado = servo 270°, encendido = servo 360° |

El joystick y la pantalla comparten el mismo bus I2C, así que se conectan a los mismos pines `SDA` y `SCL`.

---

## 📥 Instalación

### 1. Clonar el repositorio

```bash
git clone https://github.com/miusarname2/JoyDuo.git
cd JoyDuo
```

### 2. Opción A: con Arduino IDE

1. Abre el archivo del sketch (`.ino`) con **Arduino IDE**.
2. Instala las librerías desde **Herramientas → Administrar bibliotecas…**:
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`
3. Selecciona tu placa en **Herramientas → Placa** (Arduino UNO o KidsUno) y el puerto correcto.
4. Pulsa **Subir**.
5. Abre el **Monitor serie** a **9600 baudios** para ver los mensajes.

### 3. Opción B: con PlatformIO

Agrega estas dependencias a tu `platformio.ini`:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 9600
lib_deps =
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
```

Luego compila y sube:

```bash
pio run -t upload
pio device monitor
```

> ℹ️ El nombre del archivo fuente puede variar según cómo lo hayas guardado en el repositorio. Ajusta las rutas al nombre real.

---

## 🚀 Cómo usarlo

1. **Deja el joystick centrado** y conecta la alimentación: durante el arranque se calibra el centro del eje Y.
2. En el monitor serie verás el centro calibrado (`Centro Y calibrado: ...`).
3. Por defecto controlas el **servo de 270°** (LED apagado).
4. **Mueve el joystick** hacia adelante o hacia atrás para mover el servo.
5. **Pulsa el joystick** para cambiar al **servo de 360°** (LED encendido). Vuelve a pulsar para regresar al de 270°.

### Resumen de controles

| Acción | Resultado |
|--------|-----------|
| Mover el joystick en el eje Y | Mueve el servo seleccionado |
| Pulsar el joystick (y soltar) | Cambia entre servo 270° y servo 360° |
| Dejar el joystick centrado | El servo se detiene |

---

## 🎮 Modos de control

### 🔶 Servo 270° (posicional)

Funciona como un control **paso a paso**: el joystick no fija el ángulo directamente, sino que lo incrementa o decrementa mientras lo mantienes inclinado.

- **Rango permitido:** de `30°` a `150°` (empieza en `90°`).
- **Zona muerta:** ±100 unidades alrededor del centro; dentro de ella no se mueve.
- **Velocidad variable:** un paso cada **100 ms** (inclinación leve) hasta cada **20 ms** (inclinación máxima).
- **Sentido:** Y por debajo del centro → el ángulo aumenta (`Moviendo izquierda`); Y por encima → el ángulo disminuye (`Moviendo derecha`).

### 🔷 Servo 360° (rotación continua)

Controla el **sentido y la velocidad** del giro mediante la anchura del pulso.

| Dirección del joystick | Estado en pantalla | Pulso |
|------------------------|--------------------|-------|
| Y por debajo del centro | `Subiendo` | `1100 – 1450 µs` |
| Centro | `Detenido` | Servo desconectado (sin pulsos) |
| Y por encima del centro | `Bajando` | `1550 – 1900 µs` |

Medidas de seguridad:

1. **Requiere centrado previo:** al seleccionar este servo, la pantalla muestra `Centra joystick`. Hasta que el joystick permanezca centrado durante **5 lecturas seguidas**, el servo no responde.
2. **Confirmación de movimiento:** se necesitan **3 lecturas consecutivas** fuera de la zona de arranque (±160) antes de empezar a girar.
3. **Histéresis:** arranca a ±160 del centro, pero se detiene al volver a ±110.
4. **Cambio de dirección seguro:** si invertes el sentido de golpe, el servo se detiene y debe volver a confirmar el movimiento.
5. **Desconexión al parar:** al detenerse se hace `detach()` para evitar temblores y consumo innecesario.

---

## 🖥️ Pantalla OLED y LED integrado

### Pantalla (se refresca cada 250 ms)

```
KidsUno - Joystick

Joystick: OK
Y: 512
Control: Servo 270
Detenido
Angulo 270: 90
```

| Mensaje | Significado |
|---------|-------------|
| `Joystick: OK` | El joystick responde por I2C |
| `Joystick: ERROR I2C` | No se detecta el joystick en `0x39` |
| `Detenido` | Ningún servo se está moviendo |
| `Moviendo izquierda / derecha` | El servo 270° está cambiando de ángulo |
| `Subiendo / Bajando` | El servo 360° está girando |
| `Centra joystick` | Servo 360° seleccionado, esperando que centres el joystick |
| `Revisa joystick` | Falla de comunicación con el joystick |

> La pantalla se muestra rotada 180° (`setRotation(2)`).

### LED integrado

| Estado del LED | Significado |
|----------------|-------------|
| Apagado | Servo 270° seleccionado |
| Encendido | Servo 360° seleccionado |
| Parpadeo rápido (150 ms) | Error: el joystick no responde |

### Monitor serie (9600 baudios)

Imprime cada 500 ms un resumen como:

```
Y=498 Centro=512  Servo seleccionado=270  Movimiento=Detenido  Angulo 270=90
```

---

## 🗂️ Estructura del código

Todo el proyecto vive en un único archivo, organizado así:

```
JoyDuo
│
├── Constantes y variables globales
│     (pines, direcciones I2C, umbrales, temporizadores, estados)
│
├── 📡 Comunicación I2C con el joystick
│     readRegister · readAxis
│
├── 🖥️ Pantalla
│     updateDisplay
│
├── ⚙️ setup()
│     Pantalla OLED · Servo 270° · Calibración del centro Y
│
└── 🔁 loop()
      Detección del joystick · Antirrebote y selección de servo
      Control del servo 360° · Control del servo 270°
      Reporte serie · Actualización de pantalla
```

---

## 🔧 Detalles técnicos

- **Joystick I2C:** se leen los registros `0x31` (botón), `0x34` (Y alto) y `0x35` (Y bajo) de la dirección `0x39`. El valor del eje se reconstruye a **10 bits** (0–1023) combinando ambos bytes: `(alto << 2) | (bajo >> 6)`.
- **Validación de lectura:** el joystick se considera disponible solo si responden el botón (valor `0` o `1`) y el eje Y.
- **Calibración:** promedio de **32 muestras** (una cada 10 ms) del eje Y en reposo. Si falla, se usa el valor por defecto `512`.
- **Antirrebote del botón:** el cambio debe mantenerse **60 ms**. Además, el botón debe haberse **soltado** al menos una vez ("armado") antes de aceptar una nueva pulsación, lo que evita cambios repetidos por mantener pulsado.
- **Mapeo de velocidad (270°):** `map()` convierte la inclinación del joystick en un intervalo de paso entre 100 ms y 20 ms.
- **Mapeo de pulso (360°):** `map()` + `constrain()` convierten la posición del eje en microsegundos para `writeMicroseconds()`.
- **Sin bloqueos:** salvo la calibración inicial, todo el control usa `millis()` en lugar de `delay()`.
- **Tolerancia a fallos:** si el joystick deja de responder, el servo 360° se desconecta, se reinicia su estado de "listo" y se exige volver a centrar al recuperarse.

---

## 🎨 Personalización

Todos los ajustes están en constantes al inicio del archivo:

| Quieres cambiar… | Constante |
|------------------|-----------|
| Pin del servo 270° | `SERVO_270_PIN` |
| Pin del servo 360° | `SERVO_360_PIN` |
| Límites del servo 270° | `SERVO_270_MIN`, `SERVO_270_MAX` |
| Tamaño de la zona muerta (270°) | `JOYSTICK_DEADZONE` |
| Sensibilidad de arranque del servo 360° | `SERVO_360_START_THRESHOLD` |
| Umbral de parada del servo 360° | `SERVO_360_STOP_THRESHOLD` |
| Sensibilidad del antirrebote | `DEBOUNCE_MS` |
| Muestras de calibración | `CALIBRATION_SAMPLES` |
| Confirmaciones de centrado / movimiento | `CENTER_CONFIRM_SAMPLES`, `MOTION_CONFIRM_SAMPLES` |
| Frecuencia de reporte serie | `REPORT_INTERVAL_MS` |
| Frecuencia de refresco de pantalla | `DISPLAY_INTERVAL_MS` |
| Velocidad de movimiento del servo 270° | Rango `100 → 20` dentro de `map()` en `loop()` |
| Invertir el sentido del joystick | Intercambia los signos de comparación del eje Y |

---

## 🩺 Solución de problemas

| Problema | Posible solución |
|----------|------------------|
| `ERROR: no responde el joystick en I2C 0x39` | Revisa que el joystick esté en el puerto I2C (`SDA`/`SCL`) y que la dirección sea `0x39`. |
| El LED parpadea rápido | Es la señal de error de I2C: revisa cables y alimentación del joystick. |
| La pantalla no enciende | Verifica la dirección `0x3C`, las conexiones I2C y que la librería `Adafruit SSD1306` esté instalada. |
| El servo 360° no responde | Centra el joystick hasta que desaparezca `Centra joystick` y luego muévelo con decisión (más allá de ±160). |
| El servo 360° gira solo en reposo | Ajusta el punto neutro del servo (potenciómetro de ajuste) o recalibra con el joystick bien centrado al arrancar. |
| El servo 270° se mueve en sentido contrario | Invierte las comparaciones del eje Y o el cableado/montaje del joystick. |
| La placa se reinicia al mover los servos | Usa una fuente externa para los servos y une los GND. |
| El centro calibrado no es ~512 | Reinicia con el joystick sin tocar; si persiste, revisa el módulo. |

---

## ⚠️ Limitaciones conocidas

- Solo se usa el **eje Y** del joystick; el eje X no se lee.
- El ángulo del servo 270° **no se guarda**: al reiniciar vuelve a `90°`.
- Las etiquetas `izquierda/derecha` y `Subiendo/Bajando` dependen de cómo montes el joystick y los servos.
- En placas donde `LED_BUILTIN` está en `D13` (como el Arduino UNO), el LED **comparte pin** con la señal del servo 360°. Si notas temblores en el servo, cambia el pin del servo o usa un LED externo.
- El servo 360° no tiene retroalimentación de posición: solo se controla velocidad y sentido.

---

## 🔮 Ideas para el futuro

- [ ] Usar también el **eje X** (por ejemplo, un servo por eje).
- [ ] Añadir un tercer servo y un selector cíclico.
- [ ] Guardar la última posición del servo 270° en la EEPROM.
- [ ] Barra gráfica del eje Y en la pantalla OLED.
- [ ] Modo "memoria" para grabar y reproducir movimientos.
- [ ] Control por Bluetooth o Wi-Fi como alternativa al joystick.
- [ ] Soporte para un segundo joystick.

---

## 🤝 Contribuir

¡Las contribuciones son bienvenidas!

1. Haz un **fork** del repositorio.
2. Crea una rama para tu mejora: `git checkout -b feature/mi-mejora`
3. Haz tus cambios y pruébalos en hardware real.
4. Haz commit: `git commit -m "Añade mi mejora"`
5. Sube la rama: `git push origin feature/mi-mejora`
6. Abre un **Pull Request** describiendo qué cambia y por qué.

**Pautas sugeridas:**

- Mantén el estilo del código (constantes descriptivas, funciones cortas).
- Evita `delay()` en el bucle principal; usa `millis()`.
- Documenta cualquier cambio de pines o dirección I2C en este README.

---

## 📄 Licencia

Distribuido bajo la Licencia **MIT**. Consulta el archivo [LICENSE](LICENSE) para más detalles.

*Copyright (c) 2026 Oscar M. Alvarez G.*

---

## 🙌 Créditos

- Proyecto creado al 100% por [**miusarname2**](https://github.com/miusarname2).
- Hecho con ❤️ para la placa **KidsUno** y el ecosistema **Arduino**.

<div align="center">

**¿Te gustó JoyDuo? ¡Dale una ⭐ al repositorio!**

[github.com/miusarname2/JoyDuo](https://github.com/miusarname2/JoyDuo)

</div>
