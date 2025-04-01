CONFIG_FILE ?= gitconfig.ini
GIT := git

# Beispiel file für den Git Setup
# Ort : gitconfig.ini
# [user]
# name = ...
# email = ...

gitConfig:
	@echo "Lese Git-Konfiguration aus $(CONFIG_FILE)..."
	@if [ -f "$(CONFIG_FILE)" ]; then \
        $(GIT) config --global user.name "$$(awk -F= '/^name/ {print $$2}' $(CONFIG_FILE) | xargs)"; \
        $(GIT) config --global user.email "$$(awk -F= '/^email/ {print $$2}' $(CONFIG_FILE) | xargs)"; \
        echo "Git-Konfiguration erfolgreich gesetzt"; \
    else \
        echo "Fehler: $(CONFIG_FILE) nicht gefunden"; \
        exit 1; \
    fi

showGitConfig:
	@$(GIT) config --global --list