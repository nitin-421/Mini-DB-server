"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.DatabaseService = void 0;
const common_1 = require("@nestjs/common");
const node_child_process_1 = require("node:child_process");
const node_fs_1 = require("node:fs");
const node_path_1 = require("node:path");
const node_util_1 = require("node:util");
const execFileAsync = (0, node_util_1.promisify)(node_child_process_1.execFile);
const projectRoot = (0, node_path_1.resolve)(__dirname, '../../..');
const engineCandidates = [
    (0, node_path_1.join)(projectRoot, 'build-web', 'Debug', 'minidb_cli.exe'),
    (0, node_path_1.join)(projectRoot, 'build', 'Debug', 'minidb_cli.exe'),
];
const dataPath = (0, node_path_1.join)(projectRoot, 'data');
let DatabaseService = class DatabaseService {
    async execute(query) {
        const enginePath = engineCandidates.find(node_fs_1.existsSync);
        if (!enginePath) {
            throw new common_1.InternalServerErrorException('MiniDB executable was not found. Build the C++ project first.');
        }
        try {
            const { stdout, stderr } = await execFileAsync(enginePath, ['--query', query], {
                cwd: projectRoot,
                timeout: 10_000,
                windowsHide: true,
            });
            const output = (stdout || stderr).trim();
            return { ok: !output.startsWith('Error:'), output: output || 'No output.' };
        }
        catch {
            throw new common_1.InternalServerErrorException('The database engine could not execute this query.');
        }
    }
    tables() {
        if (!(0, node_fs_1.existsSync)(dataPath))
            return { tables: [] };
        return { tables: (0, node_fs_1.readdirSync)(dataPath).filter((name) => name.endsWith('.tbl')).map((name) => name.slice(0, -4)) };
    }
};
exports.DatabaseService = DatabaseService;
exports.DatabaseService = DatabaseService = __decorate([
    (0, common_1.Injectable)()
], DatabaseService);
//# sourceMappingURL=database.service.js.map