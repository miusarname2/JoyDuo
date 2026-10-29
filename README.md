# Reporte: error de PlatformIO al cargar en KidsUno

## Resumen

El proyecto está configurado para un Arduino UNO compatible, como la placa KidsBits KidsUno:

- Plataforma: `atmelavr`
- Board: `uno`
- Framework: `arduino`

La configuración está en `platformio.ini`. El error de carga no lo causó el código ni la pantalla OLED. PlatformIO tenía instalada globalmente una plataforma adicional llamada `arduinoq` (Arduino Uno Q), que no es compatible con Windows. Al intentar cargar el entorno `uno`, esa instalación incompatible interrumpía PlatformIO antes de compilar o subir el firmware.

## Reparación aplicada y comprobada

1. Se comprobó que `platformio.ini` usaba `platform = atmelavr` y `board = uno`.
2. Se identificó la plataforma global incompatible en `%USERPROFILE%\.platformio\platforms\arduinoq`.
3. Como el comando de desinstalación de PlatformIO también fallaba al cargar esa plataforma, se eliminó únicamente esa carpeta global después de verificar en su `platform.json` que correspondía a **Arduino Q**.
4. No se modificó ni eliminó la plataforma `atmelavr` ni la configuración del proyecto.
5. Se ejecutó la carga de verificación. PlatformIO detectó la placa en `COM3`, escribió el firmware y verificó la memoria flash. El resultado fue `SUCCESS`.

La placa KidsUno de este proyecto es una placa basada en ATmega328P compatible con el perfil UNO; no es una Arduino Uno Q. La plataforma `arduinoq` solo debe quitarse de otro PC si ese PC presenta el mismo error y no se necesita compilar proyectos Arduino Uno Q.

## Cómo comprobar otro PC

Abre PowerShell en la carpeta del proyecto y revisa la configuración:

```powershell
Get-Content .\platformio.ini
```

Debe incluir:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
```

Prueba la compilación:

```powershell
$pio = Join-Path $env:USERPROFILE '.platformio\penv\Scripts\platformio.exe'
& $pio run -e uno
```

Si aparece el error que menciona `arduinoq` y `windows_amd64`, inspecciona el paquete global:

```powershell
$arduinoq = Join-Path $env:USERPROFILE '.platformio\platforms\arduinoq'
Test-Path $arduinoq
Get-Content (Join-Path $arduinoq 'platform.json') -TotalCount 8
```

Solo si el archivo confirma que es la plataforma `arduinoq` y no se usa Arduino Uno Q en ese PC, quítala de la instalación global:

```powershell
& $pio platform uninstall arduinoq
```

Si ese comando falla con el mismo error al cargar `arduinoq`, y solo después de comprobar la ruta y el nombre del paquete como arriba, se puede quitar esa carpeta exacta:

```powershell
Remove-Item -LiteralPath $arduinoq -Recurse -Force
```

No elimines la carpeta global completa `.platformio`, ni `platforms\atmelavr`, ni los archivos del proyecto. Después, vuelve a compilar:

```powershell
& $pio run -e uno
```

Y carga la placa:

```powershell
& $pio run --target upload --environment uno
```

El puerto COM puede ser distinto en el otro PC. Si la detección automática falla, identifica el COM de la KidsUno antes de configurar un puerto manualmente.

## Prompt para pedir la reparación en otro PC

```text
Estoy trabajando con un proyecto PlatformIO para una placa KidsBits KidsUno, compatible con Arduino UNO clásico (ATmega328P). Al subir desde Windows, PlatformIO muestra:

UserSideException: The 'arduinoq' platform does not support the 'windows_amd64' host.

Diagnostica y repara el entorno de PlatformIO sin cambiar el firmware ni la configuración correcta del proyecto. Primero confirma que platformio.ini usa platform = atmelavr, board = uno y framework = arduino. Luego inspecciona la plataforma global %USERPROFILE%\.platformio\platforms\arduinoq y confirma en platform.json que es la plataforma de Arduino Uno Q. Si esta PC no necesita compilar Uno Q, desinstala solo esa plataforma; si el comando oficial falla por el mismo error, elimina únicamente la carpeta exacta arduinoq después de verificar su ruta y contenido. No borres .platformio, atmelavr, .pio ni ningún otro paquete. Si se usa Arduino Uno Q en este PC, detente y explícame las alternativas antes de quitarla. Finalmente ejecuta `platformio run -e uno`; si la placa está conectada, confirma el puerto y ejecuta la carga, verificando el resultado de avrdude. Informa claramente qué cambiaste y qué quedó verificado.
```
