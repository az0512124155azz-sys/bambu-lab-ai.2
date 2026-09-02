# LayerMind 3D

LayerMind 3D is the modeling workspace embedded in Bambu Studio. Press
`Ctrl+Shift+1` to open it. The existing Bambu Studio canvas is the authoritative
3D preview; generated STL geometry is imported into that canvas for slicing.

## Connections

- NVIDIA and any OpenAI-compatible cloud endpoint can be configured by URL,
  model name and an in-memory API key.
- Ollama is detected at `127.0.0.1:11434`.
- Local OpenAI-compatible services are probed at ports `8000` and `7900`.
- Fusion 360 and Blender are discovered through the selected MCP JSON file.
  Copy `resources/ai/mcp-modeling.example.json`, replace the placeholder
  commands with the commands supplied by the MCP servers, then choose the file
  in **Connections**.

MCP discovery does not grant silent control. A modeling operation must be
confirmed, and the application must receive success from the MCP tool before it
can report that Fusion 360 or Blender changed a model.

## Local history

Chat text is stored on the computer in the Bambu Studio user-data directory
under `layermind/chat-history.txt`. It does not require an account or database.
API keys are deliberately not written to that file or the application config.

## Installer

After downloading and extracting the complete Windows build artifact, run
`installer/Install-LayerMind-3D.bat`. It copies the portable application into
the current Windows user's Local AppData folder and creates a desktop shortcut.
