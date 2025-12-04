import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp", "semantic_types.h",
"TypeChecker.cpp"]

# Compilar el proyecto principal
compile_cmd = ["g++"] + programa
print("Compilando:", " ".join(compile_cmd))
result = subprocess.run(compile_cmd, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Directorios
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

for i in range(1, 6):
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)

    if not os.path.isfile(filepath):
        print(filename, "no encontrado en", input_dir)
        continue

    print(f"\n=== Ejecutando {filename} con ./a.out ===")
    run_cmd = ["./a.out", filepath]
    result = subprocess.run(run_cmd, capture_output=True, text=True)

    print("stdout:")
    print(result.stdout)
    print("stderr:")
    print(result.stderr)
    print("returncode:", result.returncode)

    # Nombre y ruta del .s que el programa genera en inputs/
    tokens_file = os.path.join(input_dir, f"input{i}.s")

    # Si existe, moverlo a outputs/ con nuevo nombre
    if os.path.isfile(tokens_file):
        dest_tokens = os.path.join(output_dir, f"input_{i}.s")
        print(f"Moviendo {tokens_file} -> {dest_tokens}")
        # Si ya existe, sobrescribimos
        if os.path.exists(dest_tokens):
            os.remove(dest_tokens)
        shutil.move(tokens_file, dest_tokens)

        # Compilar el .s movido (genera un ejecutable)
        exec_path = os.path.join(output_dir, f"input_{i}.exec")
        compile_s_cmd = ["g++", dest_tokens, "-o", exec_path]
        print("Compilando assembly:", " ".join(compile_s_cmd))
        compile_s = subprocess.run(compile_s_cmd, capture_output=True, text=True)

        # Archivo donde guardaremos el resultado de la compilación/ejecución
        run_output_file = os.path.join(output_dir, f"input_{i}_run.txt")
        with open(run_output_file, "w", encoding="utf-8") as fout:
            fout.write(f"=== Resultado de compilar {os.path.basename(dest_tokens)} ===\n")
            fout.write(f"Comando: {' '.join(compile_s_cmd)}\n")
            fout.write(f"returncode: {compile_s.returncode}\n")
            if compile_s.stdout:
                fout.write("--- compile stdout ---\n")
                fout.write(compile_s.stdout + "\n")
            if compile_s.stderr:
                fout.write("--- compile stderr ---\n")
                fout.write(compile_s.stderr + "\n")

            # Si compiló correctamente, ejecutarlo y guardar su salida
            if compile_s.returncode == 0 and os.path.isfile(exec_path):
                print("Ejecución del ejecutable:", exec_path)
                run_exec = subprocess.run([exec_path], capture_output=True, text=True)

                fout.write("\n=== Ejecución del ejecutable ===\n")
                fout.write(f"Comando: {exec_path}\n")
                fout.write(f"returncode: {run_exec.returncode}\n")
                fout.write("--- stdout ---\n")
                fout.write(run_exec.stdout + "\n")
                fout.write("--- stderr ---\n")
                fout.write(run_exec.stderr + "\n")
            else:
                fout.write("\nNo se ejecutó el binario (falló la compilación del .s o no existe el ejecutable).\n")

        print(f"Resultado guardado en {run_output_file}")

    else:
        print(f"No se creó {tokens_file} para {filename}")
