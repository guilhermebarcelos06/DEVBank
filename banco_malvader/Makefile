# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I./src

# Diretórios
SRC_DIR = src
BIN_DIR = .

# Arquivos fonte e objetos
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Nome do executável
TARGET = $(BIN_DIR)/DEVBank.exe

# Regra padrão
all: $(TARGET)

# Regra para gerar o executável
$(TARGET): $(OBJ_FILES)
	@echo "🔧 Ligando tudo em $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para compilar cada .cpp em .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "🧱 Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza dos objetos e executável
clean:
	@echo "🧹 Limpando arquivos objeto..."
	rm -f $(SRC_DIR)/*.o

mrproper: clean
	@echo "🗑️ Removendo executável..."
	rm -f $(TARGET)

# Executar o programa
run: all
	@echo "🚀 Executando o programa..."
	./$(TARGET)
