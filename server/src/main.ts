import { ValidationPipe } from '@nestjs/common';
import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);
  const env = (globalThis as {
    process?: { env?: Record<string, string | undefined> };
  }).process?.env;

  app.enableCors({
    origin: env?.CLIENT_URL || 'http://localhost:5173',
  });

  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,
      transform: true,
    }),
  );

  const port = Number(env?.CLIENT_URL ?? 3000);

  await app.listen(port, '0.0.0.0');
}
bootstrap();
