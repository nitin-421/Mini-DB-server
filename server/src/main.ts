import { ValidationPipe } from '@nestjs/common';
import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';

async function bootstrap() {
	const app = await NestFactory.create(AppModule);
	app.enableCors({ origin: 'http://localhost:5173' });
	app.useGlobalPipes(new ValidationPipe({ whitelist: true, transform: true }));
	const port = Number(
		(globalThis as { process?: { env?: { PORT?: string } } }).process?.env
			?.PORT ?? 3000,
	);
	await app.listen(port);
}
bootstrap();
