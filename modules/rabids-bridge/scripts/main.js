const BRIDGE_DIR = 'Data/rabids-bridge';
const USERS_FILE = 'Data/rabids-bridge/users.json';
const ACTORS_FILE = 'Data/rabids-bridge/actors.json';
const COMMANDS_DIR = 'Data/rabids-bridge/commands';
const DONE_DIR = 'Data/rabids-bridge/commands/done';
const CONFIG_FILE = 'Data/rabids-bridge/config.json';

function ensureDir(dir) {
  try {
    const fs = require('fs');
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
  } catch (e) {
    console.error('R.A.B.I.D.S Bridge: Failed to create directory', dir, e);
  }
}

function readJson(file) {
  try {
    const fs = require('fs');
    if (!fs.existsSync(file)) return null;
    return JSON.parse(fs.readFileSync(file, 'utf8'));
  } catch (e) {
    console.error('R.A.B.I.D.S Bridge: Failed to read', file, e);
    return null;
  }
}

function writeJson(file, data) {
  try {
    const fs = require('fs');
    fs.writeFileSync(file, JSON.stringify(data, null, 2), 'utf8');
  } catch (e) {
    console.error('R.A.B.I.D.S Bridge: Failed to write', file, e);
  }
}

function exportUsers() {
  if (!game?.users) return;
  const users = game.users.contents.map(u => ({
    _id: u.id,
    name: u.name,
    role: u.role,
    character: u.character?.id || null,
    color: u.color,
    flags: u.flags,
    _stats: u._stats
  }));
  writeJson(USERS_FILE, users);
  console.log(`R.A.B.I.D.S Bridge: Exported ${users.length} users`);
}

function exportActors() {
  if (!game?.actors) return;
  const actors = game.actors.contents.map(a => ({
    _id: a.id,
    name: a.name,
    type: a.type,
    img: a.img,
    system: a.system,
    items: a.items.contents.map(i => ({
      _id: i.id,
      name: i.name,
      type: i.type,
      system: i.system
    })),
    flags: a.flags,
    ownership: a.ownership,
    folder: a.folder?.id || null,
    _stats: a._stats
  }));
  writeJson(ACTORS_FILE, actors);
  console.log(`R.A.B.I.D.S Bridge: Exported ${actors.length} actors`);
}

function processCommands() {
  const fs = require('fs');
  ensureDir(DONE_DIR);
  if (!fs.existsSync(COMMANDS_DIR)) return;

  const files = fs.readdirSync(COMMANDS_DIR).filter(f => f.endsWith('.json'));
  for (const file of files) {
    const cmd = readJson(`${COMMANDS_DIR}/${file}`);
    if (!cmd) continue;

    try {
      executeCommand(cmd);
      fs.renameSync(`${COMMANDS_DIR}/${file}`, `${DONE_DIR}/${file}`);
      console.log(`R.A.B.I.D.S Bridge: Executed command ${cmd.action} from ${file}`);
    } catch (e) {
      console.error(`R.A.B.I.D.S Bridge: Failed command ${file}:`, e);
      writeJson(`${COMMANDS_DIR}/${file}.error`, { error: e.message, command: cmd });
    }
  }
}

function executeCommand(cmd) {
  switch (cmd.action) {
    case 'registerUser': {
      const existing = game.users.find(u => u.name === cmd.data.name);
      if (existing) {
        existing.update({ password: cmd.data.password });
      } else {
        game.users.create({
          name: cmd.data.name,
          password: cmd.data.password,
          role: CONST.USER_ROLES.PLAYER,
          flags: { rabids: { discordId: cmd.data.discordId } }
        });
      }
      exportUsers();
      break;
    }
    case 'updateUser': {
      const user = game.users.get(cmd.data.id);
      if (user) {
        user.update(cmd.data.changes);
        exportUsers();
      }
      break;
    }
    case 'updateActor': {
      const actor = game.actors.get(cmd.data.id);
      if (actor) {
        actor.update(cmd.data.changes);
        exportActors();
      }
      break;
    }
    default:
      console.warn(`R.A.B.I.D.S Bridge: Unknown action ${cmd.action}`);
  }
}

Hooks.on('ready', () => {
  console.log('R.A.B.I.D.S Bridge: Module initialized');
  ensureDir(BRIDGE_DIR);
  ensureDir(COMMANDS_DIR);
  ensureDir(DONE_DIR);

  exportUsers();
  exportActors();
  processCommands();
  console.log('R.A.B.I.D.S Bridge: Sync complete');
});

Hooks.on('createUser', () => exportUsers());
Hooks.on('updateUser', () => exportUsers());
Hooks.on('deleteUser', () => exportUsers());

Hooks.on('createActor', () => exportActors());
Hooks.on('updateActor', () => exportActors());
Hooks.on('deleteActor', () => exportActors());
