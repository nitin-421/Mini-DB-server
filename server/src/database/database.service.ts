import { Injectable, InternalServerErrorException } from '@nestjs/common';
import { execFile } from 'node:child_process';
import { existsSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { promisify } from 'node:util';

const execFileAsync = promisify(execFile);
const projectRoot = resolve(__dirname, '../../..');
const engineCandidates = [
  join(projectRoot, 'build-web', 'Debug', 'minidb_cli.exe'),
  join(projectRoot, 'build', 'Debug', 'minidb_cli.exe'),
];
const dataPath = join(projectRoot, 'data');

@Injectable()
export class DatabaseService {
  async execute(query: string) {
    const enginePath = engineCandidates.find(existsSync);
    if (!enginePath) {
      throw new InternalServerErrorException('MiniDB executable was not found. Build the C++ project first.');
    }
    try {
      const { stdout, stderr } = await execFileAsync(enginePath, ['--query', query], {
        cwd: projectRoot,
        timeout: 10_000,
        windowsHide: true,
      });
      const output = (stdout || stderr).trim();
      return { ok: !output.startsWith('Error:'), output: output || 'No output.' };
    } catch {
      throw new InternalServerErrorException('The database engine could not execute this query.');
    }
  }

  tables() {
    if (!existsSync(dataPath)) return { tables: [] };
    return { tables: readdirSync(dataPath).filter((name) => name.endsWith('.tbl')).map((name) => name.slice(0, -4)) };
  }
}
