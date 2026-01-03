import matplotlib
matplotlib.use('Agg') # Modo sin ventana (Headless)
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys
import glob
import os
import gc # Garbage Collector para liberar RAM
import resource # Para monitorear memoria

# ================= CONFIGURACION =================
NUM_ARCHIVOS = 55 # Intenta leer todos los disponibles
PATRON = "output_*.csv"

# IMPORTANTE: Cantidad de puntos para los Scatter Plots.
# 100,000 es suficiente para ver la distribucion y mantiene la RAM baja.
# Subir esto a mas de 1 millon hara el proceso lento y pesado.
MAX_PUNTOS_SCATTER = 500000
# =================================================

def check_memory():
    """Imprime el uso de memoria actual en GB"""
    usage = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    # En Linux, ru_maxrss suele ser en KB.
    gb_usage = usage / 1024.0 / 1024.0
    print "[MEMORIA] Uso actual aprox: {:.2f} GB".format(gb_usage)
    if gb_usage > 95.0:
        print "ALERTA CRITICA: Uso de RAM cercano a 100GB. Abortando para evitar colapso."
        sys.exit(1)
      
# --- 1. Busqueda de Archivos ---
todos_los_archivos = sorted(glob.glob(PATRON))
print "Archivos detectados:", len(todos_los_archivos)

if len(todos_los_archivos) == 0:
    sys.exit("Error: No hay archivos .csv")

lista_archivos = todos_los_archivos[:NUM_ARCHIVOS]

def parse_array_cell(cell_str):
    if pd.isna(cell_str) or str(cell_str).strip() == "":
        return np.array([])
    try:
        return np.fromstring(str(cell_str), sep=';')
    except:
        return np.array([])

# --- 2. Bucle de Lectura (4 Variables) ---
lista_pt = []
lista_dxy = []
lista_eta = []
lista_phi = []
archivos_leidos = 0

print "Iniciando lectura de Pt, Dxy, Eta, Phi..."
check_memory()

for archivo in lista_archivos:
    try:
        # Leemos las 4 columnas necesarias
        # Usamos usecols para no cargar basura extra en RAM
        df_temp = pd.read_csv(archivo,
                              usecols=['PF_Pt', 'PF_Dxy', 'PF_Eta', 'PF_Phi'],
                              error_bad_lines=False, warn_bad_lines=False)

        # Parsing de las 4 variables
        series_pt = df_temp['PF_Pt'].astype(str).apply(parse_array_cell)
        series_dxy = df_temp['PF_Dxy'].astype(str).apply(parse_array_cell)
        series_eta = df_temp['PF_Eta'].astype(str).apply(parse_array_cell)
        series_phi = df_temp['PF_Phi'].astype(str).apply(parse_array_cell)

        if len(series_pt) > 0:
            flat_pt = np.concatenate(series_pt.values)
            flat_dxy = np.concatenate(series_dxy.values)
            flat_eta = np.concatenate(series_eta.values)
            flat_phi = np.concatenate(series_phi.values)
          
            # Verificacion de integridad: Todas las arrays deben medir lo mismo
            largo = len(flat_pt)
            if (len(flat_dxy) == largo) and (len(flat_eta) == largo) and (len(flat_phi) == largo):
                lista_pt.append(flat_pt)
                lista_dxy.append(flat_dxy)
                lista_eta.append(flat_eta)
                lista_phi.append(flat_phi)
                archivos_leidos += 1
            else:
                print "Descarte", archivo, ": Longitudes de columnas no coinciden."

        del df_temp
        # Forzamos recoleccion de basura cada cierto tiempo
        if archivos_leidos % 5 == 0:
            gc.collect()

    except Exception as e:
        print "Error leyendo", archivo, ":", e
        continue

if len(lista_pt) == 0:
    sys.exit("Error: No hay datos validos.")

print "Uniendo arrays masivos..."
check_memory() # Chequeo antes de la concatenacion

# Concatenamos todo (Aqui es donde mas sube la RAM)
total_pt = np.concatenate(lista_pt)
total_dxy = np.concatenate(lista_dxy)
total_eta = np.concatenate(lista_eta)
total_phi = np.concatenate(lista_phi)

# Borramos las listas inmediatamente
del lista_pt, lista_dxy, lista_eta, lista_phi
gc.collect()

print "Total bruto de particulas:", len(total_pt)

# --- 3. Limpieza Sincronizada ---
print "Limpiando datos (NaNs e Infinitos)..."
mask_valid = (np.isfinite(total_pt) &
              np.isfinite(total_dxy) &
              np.isfinite(total_eta) &
              np.isfinite(total_phi) &
              (total_pt > 0))

# Aplicamos mascara
clean_pt = total_pt[mask_valid]
clean_dxy = total_dxy[mask_valid]
clean_eta = total_eta[mask_valid]
clean_phi = total_phi[mask_valid]

# Borramos los sucios
del total_pt, total_dxy, total_eta, total_phi
gc.collect()

print "Datos limpios finales:", len(clean_pt)
check_memory()

# ================= GRAFICO 1: HISTOGRAMA PT (Usa todos los datos) =================
print "Generando Grafico 1: Histograma..."
plt.figure(figsize=(10, 7))

plt.hist(clean_pt, bins=100, color='steelblue', alpha=0.8,
         label='Datos limpios (' + str(archivos_leidos) + ' archivos)')

plt.yscale('log')
plt.xlabel('Particle Flow p_T [GeV]')
plt.ylabel('Entradas (Log)')
plt.title('Distribucion de p_T Total')
plt.legend()
plt.grid(True, which="both", ls="-", alpha=0.2)

plt.savefig('grafico_1_histograma_pt.png', dpi=300)
plt.close()

# ================= PREPARACION PARA SCATTERS (OPTIMIZACION RAM) =================
print "Preparando datos ligeros para Scatter Plots..."

# AQUI ESTA LA CLAVE PARA NO PASAR LOS 100GB:
# Tomamos una muestra representativa y BORRAMOS los arrays gigantes.

if len(clean_pt) > MAX_PUNTOS_SCATTER:
    print "Submuestreando a", MAX_PUNTOS_SCATTER, "puntos aleatorios..."
    indices_random = np.random.choice(len(clean_pt), MAX_PUNTOS_SCATTER, replace=False)

    # Creamos arrays pequenos
    sample_pt = clean_pt[indices_random]
    sample_dxy = clean_dxy[indices_random]
    sample_eta = clean_eta[indices_random]
    sample_phi = clean_phi[indices_random]

    # IMPORTANTE: Liberamos la memoria masiva.
    print "Liberando memoria masiva..."
    del clean_pt, clean_dxy, clean_eta, clean_phi
    gc.collect()
    check_memory()

else:
    # Si hay pocos datos, usamos todo
    sample_pt, sample_dxy, sample_eta, sample_phi = clean_pt, clean_dxy, clean_eta, clean_phi

# ================= GRAFICO 2: SCATTER PT vs DXY =================
print "Generando Grafico 2: Scatter Pt vs Dxy..."
plt.figure(figsize=(10, 7))

plt.scatter(sample_pt, sample_dxy, s=2, alpha=0.3, color="darkblue")

plt.xlabel("PF_Pt (Transverse Momentum)")
plt.ylabel("PF_Dxy (Displacement)")
plt.title("Scatter Plot: Pt vs Dxy (Muestra)")
plt.xscale("log")
plt.grid(True, which="both", linestyle="--", linewidth=0.5)

plt.savefig('grafico_2_scatter_pt_dxy_last.png', dpi=300)
plt.close()

# ================= GRAFICO 3: ETA vs PHI (Coloreado por Dxy) =================
print "Generando Grafico 3: Eta vs Phi..."
plt.figure(figsize=(10, 8))

# Scatter: X=Eta, Y=Phi, c=Color basado en Dxy
# Usamos los arrays de muestra (sample_)
sc = plt.scatter(sample_eta, sample_phi, c=sample_dxy,
                 s=5, cmap="coolwarm", alpha=0.6, vmin=-0.1, vmax=0.1)

# Barra de color
cbar = plt.colorbar(sc)
cbar.set_label("PF_Dxy Value")

plt.xlabel("PF_Eta")
plt.ylabel("PF_Phi")
plt.title("Jet Constitution: Eta-Phi Plane (Colored by Dxy)")
plt.grid(True, linestyle="--", alpha=0.5)

# Guardar
plt.savefig('grafico_3_scatter_eta_phi_last.png', dpi=300)
plt.close()

print "--- PROCESO FINALIZADO CON EXITO ---"
check_memory()
sys.exit(0)

